// SPDX-License-Identifier: (GPL-2.0+ OR MIT)
/*
 * Copyright (c) 2022 Rockchip Electronics Co., Ltd
 *
 * author:
 *	Ding Wei, leo.ding@rock-chips.com
 *
 */

#include <asm/cacheflush.h>
#include <linux/delay.h>
#include <linux/devfreq.h>
#include <linux/devfreq_cooling.h>
#include <linux/iopoll.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/of_platform.h>
#include <linux/of_address.h>
#include <linux/slab.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/regmap.h>
#include <linux/regulator/consumer.h>
#include <linux/proc_fs.h>
#include <linux/pm_runtime.h>
#include <linux/nospec.h>
#include <linux/workqueue.h>
#include <linux/dma-iommu.h>
#include <soc/rockchip/pm_domains.h>
#include <soc/rockchip/rockchip_ipa.h>
#include <soc/rockchip/rockchip_opp_select.h>
#include <soc/rockchip/rockchip_system_monitor.h>

#include "mpp_debug.h"
#include "mpp_iommu.h"
#include "mpp_common.h"

#define RKVENC_DRIVER_NAME			"mpp_rkvenc2"

#define	RKVENC_SESSION_MAX_BUFFERS		40
#define RKVENC_MAX_CORE_NUM			4

#define to_rkvenc_info(info)		\
		container_of(info, struct rkvenc_hw_info, hw)
#define to_rkvenc_task(ctx)		\
		container_of(ctx, struct rkvenc_task, mpp_task)
#define to_rkvenc_dev(dev)		\
		container_of(dev, struct rkvenc_dev, mpp)


enum RKVENC_FORMAT_TYPE {
	RKVENC_FMT_BASE		= 0x0000,
	RKVENC_FMT_H264E	= RKVENC_FMT_BASE + 0,
	RKVENC_FMT_H265E	= RKVENC_FMT_BASE + 1,
	RKVENC_FMT_JPEGE	= RKVENC_FMT_BASE + 2,

	RKVENC_FMT_OSD_BASE	= 0x1000,
	RKVENC_FMT_H264E_OSD	= RKVENC_FMT_OSD_BASE + 0,
	RKVENC_FMT_H265E_OSD	= RKVENC_FMT_OSD_BASE + 1,
	RKVENC_FMT_JPEGE_OSD	= RKVENC_FMT_OSD_BASE + 2,
	RKVENC_FMT_BUTT,
};

enum RKVENC_CLASS_TYPE {
	RKVENC_CLASS_BASE	= 0,	/* base */
	RKVENC_CLASS_PIC	= 1,	/* picture configure */
	RKVENC_CLASS_RC		= 2,	/* rate control */
	RKVENC_CLASS_PAR	= 3,	/* parameter */
	RKVENC_CLASS_SQI	= 4,	/* subjective Adjust */
	RKVENC_CLASS_SCL	= 5,	/* scaling list */
	RKVENC_CLASS_OSD	= 6,	/* osd */
	RKVENC_CLASS_ST		= 7,	/* status */
	RKVENC_CLASS_DEBUG	= 8,	/* debug */
	RKVENC_CLASS_BUTT,
};

enum RKVENC_CLASS_FD_TYPE {
	RKVENC_CLASS_FD_BASE	= 0,	/* base */
	RKVENC_CLASS_FD_OSD	= 1,	/* osd */
	RKVENC_CLASS_FD_BUTT,
};

struct rkvenc_reg_msg {
	u32 base_s;
	u32 base_e;
};

struct rkvenc_hw_info {
	struct mpp_hw_info hw;
	/* for register range check */
	u32 reg_class;
	struct rkvenc_reg_msg reg_msg[RKVENC_CLASS_BUTT];
	/* for fd translate */
	u32 fd_class;
	struct {
		u32 class;
		u32 base_fmt;
	} fd_reg[RKVENC_CLASS_FD_BUTT];
	/* for get format */
	struct {
		u32 class;
		u32 base;
		u32 bitpos;
		u32 bitlen;
	} fmt_reg;
	/* register info */
	u32 enc_start_base;
	u32 enc_clr_base;
	u32 int_en_base;
	u32 int_mask_base;
	u32 int_clr_base;
	u32 int_sta_base;
	u32 enc_wdg_base;
	u32 err_mask;
};

struct rkvenc_task {
	struct mpp_task mpp_task;
	int fmt;
	struct rkvenc_hw_info *hw_info;

	/* class register */
	struct {
		u32 valid;
		u32 *data;
		u32 size;
	} reg[RKVENC_CLASS_BUTT];
	/* register offset info */
	struct reg_offset_info off_inf;

	enum MPP_CLOCK_MODE clk_mode;
	u32 irq_status;
	/* req for current task */
	u32 w_req_cnt;
	struct mpp_request w_reqs[MPP_MAX_MSG_NUM];
	u32 r_req_cnt;
	struct mpp_request r_reqs[MPP_MAX_MSG_NUM];
	struct mpp_dma_buffer *table;
	u32 task_no;
};

#define RKVENC_MAX_RCB_NUM		(4)

struct rcb_info_elem {
	u32 index;
	u32 size;
};

struct rkvenc2_rcb_info {
	u32 cnt;
	struct rcb_info_elem elem[RKVENC_MAX_RCB_NUM];
};

struct rkvenc2_session_priv {
	struct rw_semaphore rw_sem;
	/* codec info from user */
	struct {
		/* show mode */
		u32 flag;
		/* item data */
		u64 val;
	} codec_info[ENC_INFO_BUTT];
	/* rcb_info for sram */
	struct rkvenc2_rcb_info rcb_inf;
};

struct rkvenc_dev {
	struct mpp_dev mpp;
	struct rkvenc_hw_info *hw_info;

	struct mpp_clk_info aclk_info;
	struct mpp_clk_info hclk_info;
	struct mpp_clk_info core_clk_info;
	u32 default_max_load;
#ifdef CONFIG_ROCKCHIP_MPP_PROC_FS
	struct proc_dir_entry *procfs;
#endif
	struct reset_control *rst_a;
	struct reset_control *rst_h;
	struct reset_control *rst_core;

	/* internal rcb-memory */
	u32 sram_size;
	u32 sram_used;
	dma_addr_t sram_iova;
	u32 sram_enabled;
	struct page *rcb_page;
};


static struct rkvenc_hw_info rkvenc_v2_hw_info = {
	.hw = {
		.reg_num = 254,
		.reg_id = 0,
		.reg_en = 4,
		.reg_start = 160,
		.reg_end = 253,
	},
	.reg_class = RKVENC_CLASS_BUTT,
	.reg_msg[RKVENC_CLASS_BASE] = {
		.base_s = 0x0000,
		.base_e = 0x0120,
	},
	.reg_msg[RKVENC_CLASS_PIC] = {
		.base_s = 0x0270,
		.base_e = 0x0480,
	},
	.reg_msg[RKVENC_CLASS_RC] = {
		.base_s = 0x1000,
		.base_e = 0x110c,
	},
	.reg_msg[RKVENC_CLASS_PAR] = {
		.base_s = 0x1700,
		.base_e = 0x19cc,
	},
	.reg_msg[RKVENC_CLASS_SQI] = {
		.base_s = 0x2000,
		.base_e = 0x20fc,
	},
	.reg_msg[RKVENC_CLASS_SCL] = {
		.base_s = 0x21e0,
		.base_e = 0x2dfc,
	},
	.reg_msg[RKVENC_CLASS_OSD] = {
		.base_s = 0x3000,
		.base_e = 0x326c,
	},
	.reg_msg[RKVENC_CLASS_ST] = {
		.base_s = 0x4000,
		.base_e = 0x424c,
	},
	.reg_msg[RKVENC_CLASS_DEBUG] = {
		.base_s = 0x5000,
		.base_e = 0x5354,
	},
	.fd_class = RKVENC_CLASS_FD_BUTT,
	.fd_reg[RKVENC_CLASS_FD_BASE] = {
		.class = RKVENC_CLASS_PIC,
		.base_fmt = RKVENC_FMT_BASE,
	},
	.fd_reg[RKVENC_CLASS_FD_OSD] = {
		.class = RKVENC_CLASS_OSD,
		.base_fmt = RKVENC_FMT_OSD_BASE,
	},
	.fmt_reg = {
		.class = RKVENC_CLASS_PIC,
		.base = 0x0300,
		.bitpos = 0,
		.bitlen = 2,
	},
	.enc_start_base = 0x0010,
	.enc_clr_base = 0x0014,
	.int_en_base = 0x0020,
	.int_mask_base = 0x0024,
	.int_clr_base = 0x0028,
	.int_sta_base = 0x002c,
	.enc_wdg_base = 0x0038,
	.err_mask = 0x27d0,
};

/*
 * file handle translate information for v2
 */
static const u16 trans_tbl_h264e_v2[] = {
	4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
	14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
	// /* renc and ref wrap */
	// 24, 25, 26, 27,
};

static const u16 trans_tbl_h264e_v2_osd[] = {
	3, 4, 12, 13, 21, 22, 30, 31,
	39, 40, 48, 49, 57, 58, 66, 67,
};

static const u16 trans_tbl_h265e_v2[] = {
	4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
	14, 15, 16, 17, 18, 19, 20, 21, 22, 23
};

static const u16 trans_tbl_h265e_v2_osd[] = {
	3, 4, 12, 13, 21, 22, 30, 31,
	39, 40, 48, 49, 57, 58, 66, 67,
};

static const u16 trans_tbl_jpege[] = {
	100, 101, 102, 103, 104, 105, 106, 107,
	108, 109, 110,
};

static const u16 trans_tbl_jpege_osd[] = {
	81, 82, 90, 91, 99, 100, 108, 109,
	117, 118, 126, 127, 135, 136, 144, 145,
};

static struct mpp_trans_info trans_rkvenc_v2[] = {
	[RKVENC_FMT_H264E] = {
		.count = ARRAY_SIZE(trans_tbl_h264e_v2),
		.table = trans_tbl_h264e_v2,
	},
	[RKVENC_FMT_H264E_OSD] = {
		.count = ARRAY_SIZE(trans_tbl_h264e_v2_osd),
		.table = trans_tbl_h264e_v2_osd,
	},
	[RKVENC_FMT_H265E] = {
		.count = ARRAY_SIZE(trans_tbl_h265e_v2),
		.table = trans_tbl_h265e_v2,
	},
	[RKVENC_FMT_H265E_OSD] = {
		.count = ARRAY_SIZE(trans_tbl_h265e_v2_osd),
		.table = trans_tbl_h265e_v2_osd,
	},
	[RKVENC_FMT_JPEGE] = {
		.count = ARRAY_SIZE(trans_tbl_jpege),
		.table = trans_tbl_jpege,
	},
	[RKVENC_FMT_JPEGE_OSD] = {
		.count = ARRAY_SIZE(trans_tbl_jpege_osd),
		.table = trans_tbl_jpege_osd,
	},
};

static bool req_over_class(struct mpp_request *req,
			   struct rkvenc_task *task, int class)
{
	bool ret;
	u32 base_s, base_e, req_e;
	struct rkvenc_hw_info *hw = task->hw_info;

	base_s = hw->reg_msg[class].base_s;
	base_e = hw->reg_msg[class].base_e;
	req_e = req->offset + req->size - sizeof(u32);

	ret = (req->offset <= base_e && req_e >= base_s) ? true : false;

	return ret;
}

static int rkvenc_free_class_msg(struct rkvenc_task *task)
{
	u32 i;
	u32 reg_class = task->hw_info->reg_class;

	for (i = 0; i < reg_class; i++) {
		kfree(task->reg[i].data);
		task->reg[i].size = 0;
	}

	return 0;
}

static int rkvenc_alloc_class_msg(struct rkvenc_task *task, int class)
{
	u32 *data;
	struct rkvenc_hw_info *hw = task->hw_info;

	if (!task->reg[class].data) {
		u32 base_s = hw->reg_msg[class].base_s;
		u32 base_e = hw->reg_msg[class].base_e;
		u32 class_size = base_e - base_s + sizeof(u32);

		data = kzalloc(class_size, GFP_KERNEL);
		if (!data)
			return -ENOMEM;
		task->reg[class].data = data;
		task->reg[class].size = class_size;
	}

	return 0;
}

static int rkvenc_update_req(struct rkvenc_task *task, int class,
			     struct mpp_request *req_in,
			     struct mpp_request *req_out)
{
	u32 base_s, base_e, req_e, s, e;
	struct rkvenc_hw_info *hw = task->hw_info;

	base_s = hw->reg_msg[class].base_s;
	base_e = hw->reg_msg[class].base_e;
	req_e = req_in->offset + req_in->size - sizeof(u32);
	s = max(req_in->offset, base_s);
	e = min(req_e, base_e);

	req_out->offset = s;
	req_out->size = e - s + sizeof(u32);
	req_out->data = (u8 *)req_in->data + (s - req_in->offset);

	return 0;
}

static int rkvenc_get_class_msg(struct rkvenc_task *task,
				u32 addr, struct mpp_request *msg)
{
	int i;
	bool found = false;
	u32 base_s, base_e;
	struct rkvenc_hw_info *hw = task->hw_info;

	if (!msg)
		return -EINVAL;

	memset(msg, 0, sizeof(*msg));
	for (i = 0; i < hw->reg_class; i++) {
		base_s = hw->reg_msg[i].base_s;
		base_e = hw->reg_msg[i].base_e;
		if (addr >= base_s && addr < base_e) {
			found = true;
			msg->offset = base_s;
			msg->size = task->reg[i].size;
			msg->data = task->reg[i].data;
			break;
		}
	}

	return (found ? 0 : (-EINVAL));
}

static u32 *rkvenc_get_class_reg(struct rkvenc_task *task, u32 addr)
{
	int i;
	u8 *reg = NULL;
	u32 base_s, base_e;
	struct rkvenc_hw_info *hw = task->hw_info;

	for (i = 0; i < hw->reg_class; i++) {
		base_s = hw->reg_msg[i].base_s;
		base_e = hw->reg_msg[i].base_e;
		if (addr >= base_s && addr < base_e) {
			reg = (u8 *)task->reg[i].data + (addr - base_s);
			break;
		}
	}

	return (u32 *)reg;
}

static int rkvenc2_extract_rcb_info(struct rkvenc2_rcb_info *rcb_inf,
				    struct mpp_request *req)
{
	int max_size = ARRAY_SIZE(rcb_inf->elem);
	int cnt = req->size / sizeof(rcb_inf->elem[0]);

	if (req->size > sizeof(rcb_inf->elem)) {
		mpp_err("count %d,max_size %d\n", cnt, max_size);
		return -EINVAL;
	}
	if (copy_from_user(rcb_inf->elem, req->data, req->size)) {
		mpp_err("copy_from_user failed\n");
		return -EINVAL;
	}
	rcb_inf->cnt = cnt;

	return 0;
}

static int rkvenc_extract_task_msg(struct mpp_session *session,
				   struct rkvenc_task *task,
				   struct mpp_task_msgs *msgs)
{
	int ret;
	u32 i, j;
	struct mpp_request *req;
	struct rkvenc_hw_info *hw = task->hw_info;

	mpp_debug_enter();

	for (i = 0; i < msgs->req_cnt; i++) {
		req = &msgs->reqs[i];
		if (!req->size)
			continue;

		switch (req->cmd) {
		case MPP_CMD_SET_REG_WRITE: {
			void *data;
			struct mpp_request *wreq;

			for (j = 0; j < hw->reg_class; j++) {
				if (!req_over_class(req, task, j))
					continue;

				ret = rkvenc_alloc_class_msg(task, j);
				if (ret) {
					mpp_err("alloc class msg %d fail.\n", j);
					goto fail;
				}
				wreq = &task->w_reqs[task->w_req_cnt];
				rkvenc_update_req(task, j, req, wreq);
				data = rkvenc_get_class_reg(task, wreq->offset);
				if (!data) {
					mpp_err("get class reg fail, offset %08x\n", wreq->offset);
					ret = -EINVAL;
					goto fail;
				}
				if (copy_from_user(data, wreq->data, wreq->size)) {
					mpp_err("copy_from_user fail, offset %08x\n", wreq->offset);
					ret = -EIO;
					goto fail;
				}
				task->reg[j].valid = 1;
				task->w_req_cnt++;
			}
		} break;
		case MPP_CMD_SET_REG_READ: {
			struct mpp_request *rreq;

			for (j = 0; j < hw->reg_class; j++) {
				if (!req_over_class(req, task, j))
					continue;

				ret = rkvenc_alloc_class_msg(task, j);
				if (ret) {
					mpp_err("alloc class msg reg %d fail.\n", j);
					goto fail;
				}
				rreq = &task->r_reqs[task->r_req_cnt];
				rkvenc_update_req(task, j, req, rreq);
				task->reg[j].valid = 1;
				task->r_req_cnt++;
			}
		} break;
		case MPP_CMD_SET_REG_ADDR_OFFSET: {
			mpp_extract_reg_offset_info(&task->off_inf, req);
		} break;
		case MPP_CMD_SET_RCB_INFO: {
			struct rkvenc2_session_priv *priv = session->priv;

			if (priv)
				rkvenc2_extract_rcb_info(&priv->rcb_inf, req);
		} break;
		default:
			break;
		}
	}
	mpp_debug(DEBUG_TASK_INFO, "w_req_cnt=%d, r_req_cnt=%d\n",
		  task->w_req_cnt, task->r_req_cnt);

	mpp_debug_enter();
	return 0;

fail:
	rkvenc_free_class_msg(task);

	mpp_debug_enter();
	return ret;
}

static int rkvenc_task_get_format(struct mpp_dev *mpp,
				  struct rkvenc_task *task)
{
	u32 offset, val;

	struct rkvenc_hw_info *hw = task->hw_info;
	u32 class = hw->fmt_reg.class;
	u32 *class_reg = task->reg[class].data;
	u32 class_size = task->reg[class].size;
	u32 class_base = hw->reg_msg[class].base_s;
	u32 bitpos = hw->fmt_reg.bitpos;
	u32 bitlen = hw->fmt_reg.bitlen;

	if (!class_reg || !class_size)
		return -EINVAL;

	offset = hw->fmt_reg.base - class_base;
	val = class_reg[offset/sizeof(u32)];
	task->fmt = (val >> bitpos) & ((1 << bitlen) - 1);

	return 0;
}


static int rkvenc2_set_rcbbuf(struct mpp_dev *mpp, struct mpp_session *session,
			      struct rkvenc_task *task)
{
	struct rkvenc_dev *enc = to_rkvenc_dev(mpp);
	struct rkvenc2_session_priv *priv = session->priv;
	u32 sram_enabled = 0;

	mpp_debug_enter();

	if (priv && enc->sram_iova) {
		int i;
		u32 *reg;
		u32 reg_idx, rcb_size, rcb_offset;
		struct rkvenc2_rcb_info *rcb_inf = &priv->rcb_inf;

		rcb_offset = 0;
		for (i = 0; i < rcb_inf->cnt; i++) {
			reg_idx = rcb_inf->elem[i].index;
			rcb_size = rcb_inf->elem[i].size;

			if (rcb_offset > enc->sram_size ||
			    (rcb_offset + rcb_size) > enc->sram_used)
				continue;

			mpp_debug(DEBUG_SRAM_INFO, "rcb: reg %d offset %d, size %d\n",
				  reg_idx, rcb_offset, rcb_size);

			reg = rkvenc_get_class_reg(task, reg_idx * sizeof(u32));
			if (reg)
				*reg = enc->sram_iova + rcb_offset;

			rcb_offset += rcb_size;
			sram_enabled = 1;
		}
	}
	if (enc->sram_enabled != sram_enabled) {
		mpp_debug(DEBUG_SRAM_INFO, "sram %s\n", sram_enabled ? "enabled" : "disabled");
		enc->sram_enabled = sram_enabled;
	}

	mpp_debug_leave();

	return 0;
}

static void *rkvenc_alloc_task(struct mpp_session *session,
			       struct mpp_task_msgs *msgs)
{
	int ret;
	struct rkvenc_task *task;
	struct mpp_task *mpp_task;
	struct mpp_dev *mpp = session->mpp;

	mpp_debug_enter();

	task = kzalloc(sizeof(*task), GFP_KERNEL);
	if (!task)
		return NULL;

	mpp_task = &task->mpp_task;
	mpp_task_init(session, mpp_task);
	mpp_task->hw_info = mpp->var->hw_info;
	task->hw_info = to_rkvenc_info(mpp_task->hw_info);
	/* extract reqs for current task */
	ret = rkvenc_extract_task_msg(session, task, msgs);
	if (ret)
		goto free_task;
	mpp_task->reg = task->reg[0].data;
	/* get format */
	ret = rkvenc_task_get_format(mpp, task);
	if (ret)
		goto free_task;
	/* process fd in register */
	if (!(msgs->flags & MPP_FLAGS_REG_FD_NO_TRANS)) {
		u32 i, j;
		int cnt;
		u32 off;
		const u16 *tbl;
		struct rkvenc_hw_info *hw = task->hw_info;

		for (i = 0; i < hw->fd_class; i++) {
			u32 class = hw->fd_reg[i].class;
			u32 fmt = hw->fd_reg[i].base_fmt + task->fmt;
			u32 *reg = task->reg[class].data;
			u32 ss = hw->reg_msg[class].base_s / sizeof(u32);

			if (!reg)
				continue;

			ret = mpp_translate_reg_address(session, mpp_task, fmt, reg, NULL);
			if (ret)
				goto fail;

			cnt = mpp->var->trans_info[fmt].count;
			tbl = mpp->var->trans_info[fmt].table;
			for (j = 0; j < cnt; j++) {
				off = mpp_query_reg_offset_info(&task->off_inf, tbl[j] + ss);
				mpp_debug(DEBUG_IOMMU, "reg[%d] + offset %d\n", tbl[j] + ss, off);
				reg[tbl[j]] += off;
			}
		}
	}
	rkvenc2_set_rcbbuf(mpp, session, task);
	task->clk_mode = CLK_MODE_NORMAL;

	mpp_debug_leave();

	return mpp_task;

fail:
	mpp_task_dump_mem_region(mpp, mpp_task);
	mpp_task_dump_reg(mpp, mpp_task);
	mpp_task_finalize(session, mpp_task);
	/* free class register buffer */
	rkvenc_free_class_msg(task);
free_task:
	kfree(task);

	return NULL;
}

static int rkvenc_run(struct mpp_dev *mpp, struct mpp_task *mpp_task)
{
	u32 i, j;
	u32 start_val = 0;
	struct rkvenc_dev *enc = to_rkvenc_dev(mpp);
	struct rkvenc_task *task = to_rkvenc_task(mpp_task);
	struct rkvenc_hw_info *hw = enc->hw_info;

	mpp_debug_enter();

	/* Add force clear to avoid pagefault */
	mpp_write(mpp, hw->enc_clr_base, 0x2);
	udelay(5);
	mpp_write(mpp, hw->enc_clr_base, 0x0);

	/* clear hardware counter */
	mpp_write_relaxed(mpp, 0x5300, 0x2);

	for (i = 0; i < task->w_req_cnt; i++) {
		int ret;
		u32 s, e, off;
		u32 *regs;

		struct mpp_request msg;
		struct mpp_request *req = &task->w_reqs[i];

		ret = rkvenc_get_class_msg(task, req->offset, &msg);
		if (ret)
			return -EINVAL;

		s = (req->offset - msg.offset) / sizeof(u32);
		e = s + req->size / sizeof(u32);
		regs = (u32 *)msg.data;
		for (j = s; j < e; j++) {
			off = msg.offset + j * sizeof(u32);
			if (off == enc->hw_info->enc_start_base) {
				start_val = regs[j];
				continue;
			}
			mpp_write_relaxed(mpp, off, regs[j]);
		}
	}

	/* init current task */
	mpp->cur_task = mpp_task;
	/* Flush the register before the start the device */
	wmb();
	mpp_write(mpp, enc->hw_info->enc_start_base, start_val);

	mpp_debug_leave();

	return 0;
}

static int rkvenc_irq(struct mpp_dev *mpp)
{
	struct rkvenc_dev *enc = to_rkvenc_dev(mpp);
	struct rkvenc_hw_info *hw = enc->hw_info;

	mpp_debug_enter();

	mpp->irq_status = mpp_read(mpp, hw->int_sta_base);
	if (!mpp->irq_status)
		return IRQ_NONE;
	mpp_write(mpp, hw->int_mask_base, 0x100);
	mpp_write(mpp, hw->int_clr_base, 0xffffffff);
	udelay(5);
	mpp_write(mpp, hw->int_sta_base, 0);

	mpp_debug_leave();

	return IRQ_WAKE_THREAD;
}

static int rkvenc_isr(struct mpp_dev *mpp)
{
	struct rkvenc_task *task;
	struct mpp_task *mpp_task;
	struct rkvenc_dev *enc = to_rkvenc_dev(mpp);

	mpp_debug_enter();

	/* FIXME use a spin lock here */
	if (!mpp->cur_task) {
		dev_err(mpp->dev, "no current task\n");
		return IRQ_HANDLED;
	}

	mpp_task = mpp->cur_task;
	mpp_time_diff(mpp_task);
	mpp->cur_task = NULL;
	task = to_rkvenc_task(mpp_task);
	task->irq_status = mpp->irq_status;
	mpp_debug(DEBUG_IRQ_STATUS, "irq_status: %08x\n", task->irq_status);

	if (task->irq_status & enc->hw_info->err_mask) {
		atomic_inc(&mpp->reset_request);
		/* dump register */
		if (mpp_debug_unlikely(DEBUG_DUMP_ERR_REG))
			mpp_task_dump_hw_reg(mpp, mpp_task);
	}
	mpp_task_finish(mpp_task->session, mpp_task);

	mpp_debug_leave();

	return IRQ_HANDLED;
}

static int rkvenc_finish(struct mpp_dev *mpp, struct mpp_task *mpp_task)
{
	u32 i, j;
	u32 *reg;
	struct rkvenc_task *task = to_rkvenc_task(mpp_task);

	mpp_debug_enter();

	for (i = 0; i < task->r_req_cnt; i++) {
		int ret;
		int s, e;
		struct mpp_request msg;
		struct mpp_request *req = &task->r_reqs[i];

		ret = rkvenc_get_class_msg(task, req->offset, &msg);
		if (ret)
			return -EINVAL;
		s = (req->offset - msg.offset) / sizeof(u32);
		e = s + req->size / sizeof(u32);
		reg = (u32 *)msg.data;
		for (j = s; j < e; j++)
			reg[j] = mpp_read_relaxed(mpp, msg.offset + j * sizeof(u32));

	}
	/* revert hack for irq status */
	reg = rkvenc_get_class_reg(task, task->hw_info->int_sta_base);
	if (reg)
		*reg = task->irq_status;

	mpp_debug_leave();

	return 0;
}

static int rkvenc_result(struct mpp_dev *mpp,
			 struct mpp_task *mpp_task,
			 struct mpp_task_msgs *msgs)
{
	u32 i;
	struct rkvenc_task *task = to_rkvenc_task(mpp_task);

	mpp_debug_enter();

	for (i = 0; i < task->r_req_cnt; i++) {
		struct mpp_request *req = &task->r_reqs[i];
		u32 *reg = rkvenc_get_class_reg(task, req->offset);

		if (!reg)
			return -EINVAL;
		if (copy_to_user(req->data, reg, req->size)) {
			mpp_err("copy_to_user reg fail\n");
			return -EIO;
		}
	}

	mpp_debug_leave();

	return 0;
}

static int rkvenc_free_task(struct mpp_session *session,
			    struct mpp_task *mpp_task)
{
	struct rkvenc_task *task = to_rkvenc_task(mpp_task);

	mpp_task_finalize(session, mpp_task);
	rkvenc_free_class_msg(task);
	kfree(task);

	return 0;
}

static int rkvenc_control(struct mpp_session *session, struct mpp_request *req)
{
	switch (req->cmd) {
	case MPP_CMD_SEND_CODEC_INFO: {
		int i;
		int cnt;
		struct codec_info_elem elem;
		struct rkvenc2_session_priv *priv;

		if (!session || !session->priv) {
			mpp_err("session info null\n");
			return -EINVAL;
		}
		priv = session->priv;

		cnt = req->size / sizeof(elem);
		cnt = (cnt > ENC_INFO_BUTT) ? ENC_INFO_BUTT : cnt;
		mpp_debug(DEBUG_IOCTL, "codec info count %d\n", cnt);
		for (i = 0; i < cnt; i++) {
			if (copy_from_user(&elem, req->data + i * sizeof(elem), sizeof(elem))) {
				mpp_err("copy_from_user failed\n");
				continue;
			}
			if (elem.type > ENC_INFO_BASE && elem.type < ENC_INFO_BUTT &&
			    elem.flag > CODEC_INFO_FLAG_NULL && elem.flag < CODEC_INFO_FLAG_BUTT) {
				elem.type = array_index_nospec(elem.type, ENC_INFO_BUTT);
				priv->codec_info[elem.type].flag = elem.flag;
				priv->codec_info[elem.type].val = elem.data;
			} else {
				mpp_err("codec info invalid, type %d, flag %d\n",
					elem.type, elem.flag);
			}
		}
	} break;
	default: {
		mpp_err("unknown mpp ioctl cmd %x\n", req->cmd);
	} break;
	}

	return 0;
}

static int rkvenc_free_session(struct mpp_session *session)
{
	if (session && session->priv) {
		kfree(session->priv);
		session->priv = NULL;
	}

	return 0;
}

static int rkvenc_init_session(struct mpp_session *session)
{
	struct rkvenc2_session_priv *priv;

	if (!session) {
		mpp_err("session is null\n");
		return -EINVAL;
	}

	priv = kzalloc(sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	init_rwsem(&priv->rw_sem);
	session->priv = priv;

	return 0;
}

#ifdef CONFIG_ROCKCHIP_MPP_PROC_FS
static int rkvenc_procfs_remove(struct mpp_dev *mpp)
{
	struct rkvenc_dev *enc = to_rkvenc_dev(mpp);

	if (enc->procfs) {
		proc_remove(enc->procfs);
		enc->procfs = NULL;
	}

	return 0;
}

static int rkvenc_dump_session(struct mpp_session *session, struct seq_file *seq)
{
	int i;
	struct rkvenc2_session_priv *priv = session->priv;

	down_read(&priv->rw_sem);
	/* item name */
	seq_puts(seq, "------------------------------------------------------");
	seq_puts(seq, "------------------------------------------------------\n");
	seq_printf(seq, "|%8s|", (const char *)"session");
	seq_printf(seq, "%8s|", (const char *)"device");
	for (i = ENC_INFO_BASE; i < ENC_INFO_BUTT; i++) {
		bool show = priv->codec_info[i].flag;

		if (show)
			seq_printf(seq, "%8s|", enc_info_item_name[i]);
	}
	seq_puts(seq, "\n");
	/* item data*/
	seq_printf(seq, "|%8p|", session);
	seq_printf(seq, "%8s|", mpp_device_name[session->device_type]);
	for (i = ENC_INFO_BASE; i < ENC_INFO_BUTT; i++) {
		u32 flag = priv->codec_info[i].flag;

		if (!flag)
			continue;
		if (flag == CODEC_INFO_FLAG_NUMBER) {
			u32 data = priv->codec_info[i].val;

			seq_printf(seq, "%8d|", data);
		} else if (flag == CODEC_INFO_FLAG_STRING) {
			const char *name = (const char *)&priv->codec_info[i].val;

			seq_printf(seq, "%8s|", name);
		} else {
			seq_printf(seq, "%8s|", (const char *)"null");
		}
	}
	seq_puts(seq, "\n");
	up_read(&priv->rw_sem);

	return 0;
}

static int rkvenc_show_session_info(struct seq_file *seq, void *offset)
{
	struct mpp_session *session = NULL, *n;
	struct mpp_dev *mpp = seq->private;

	mutex_lock(&mpp->srv->session_lock);
	list_for_each_entry_safe(session, n,
				 &mpp->srv->session_list,
				 session_link) {
		if (session->device_type != MPP_DEVICE_RKVENC)
			continue;
		if (!session->priv)
			continue;
		if (mpp->dev_ops->dump_session)
			mpp->dev_ops->dump_session(session, seq);
	}
	mutex_unlock(&mpp->srv->session_lock);

	return 0;
}

static int rkvenc_procfs_init(struct mpp_dev *mpp)
{
	struct rkvenc_dev *enc = to_rkvenc_dev(mpp);

	enc->procfs = proc_mkdir(mpp->dev->of_node->name, mpp->srv->procfs);
	if (IS_ERR_OR_NULL(enc->procfs)) {
		mpp_err("failed on open procfs\n");
		enc->procfs = NULL;
		return -EIO;
	}
	/* for debug */
	mpp_procfs_create_u32("aclk", 0644,
			      enc->procfs, &enc->aclk_info.debug_rate_hz);
	mpp_procfs_create_u32("clk_core", 0644,
			      enc->procfs, &enc->core_clk_info.debug_rate_hz);
	mpp_procfs_create_u32("session_buffers", 0644,
			      enc->procfs, &mpp->session_max_buffers);
	/* for show session info */
	proc_create_single_data("sessions-info", 0444,
				enc->procfs, rkvenc_show_session_info, mpp);

	return 0;
}

#else
static inline int rkvenc_procfs_remove(struct mpp_dev *mpp)
{
	return 0;
}

static inline int rkvenc_procfs_init(struct mpp_dev *mpp)
{
	return 0;
}

#endif

static int rkvenc_init(struct mpp_dev *mpp)
{
	struct rkvenc_dev *enc = to_rkvenc_dev(mpp);
	int ret = 0;

	mpp->grf_info = &mpp->srv->grf_infos[MPP_DRIVER_RKVENC];

	/* Get clock info from dtsi */
	ret = mpp_get_clk_info(mpp, &enc->aclk_info, "aclk_vcodec");
	if (ret)
		mpp_err("failed on clk_get aclk_vcodec\n");
	ret = mpp_get_clk_info(mpp, &enc->hclk_info, "hclk_vcodec");
	if (ret)
		mpp_err("failed on clk_get hclk_vcodec\n");
	ret = mpp_get_clk_info(mpp, &enc->core_clk_info, "clk_core");
	if (ret)
		mpp_err("failed on clk_get clk_core\n");
	/* Get normal max workload from dtsi */
	of_property_read_u32(mpp->dev->of_node,
			     "rockchip,default-max-load",
			     &enc->default_max_load);
	/* Set default rates */
	mpp_set_clk_info_rate_hz(&enc->aclk_info, CLK_MODE_DEFAULT, 300 * MHZ);
	mpp_set_clk_info_rate_hz(&enc->core_clk_info, CLK_MODE_DEFAULT, 600 * MHZ);

	/* Get reset control from dtsi */
	enc->rst_a = mpp_reset_control_get(mpp, RST_TYPE_A, "video_a");
	if (!enc->rst_a)
		mpp_err("No aclk reset resource define\n");
	enc->rst_h = mpp_reset_control_get(mpp, RST_TYPE_H, "video_h");
	if (!enc->rst_h)
		mpp_err("No hclk reset resource define\n");
	enc->rst_core = mpp_reset_control_get(mpp, RST_TYPE_CORE, "video_core");
	if (!enc->rst_core)
		mpp_err("No core reset resource define\n");

	return 0;
}

static int rkvenc_reset(struct mpp_dev *mpp)
{
	struct rkvenc_dev *enc = to_rkvenc_dev(mpp);
	struct rkvenc_hw_info *hw = enc->hw_info;

	mpp_debug_enter();

	/* safe reset */
	mpp_write(mpp, hw->int_mask_base, 0x3FF);
	mpp_write(mpp, hw->enc_clr_base, 0x1);
	udelay(5);
	mpp_write(mpp, hw->int_clr_base, 0xffffffff);
	mpp_write(mpp, hw->int_sta_base, 0);

	/* cru reset */
	if (enc->rst_a && enc->rst_h && enc->rst_core) {
		rockchip_pmu_idle_request(mpp->dev, true);
		mpp_safe_reset(enc->rst_a);
		mpp_safe_reset(enc->rst_h);
		mpp_safe_reset(enc->rst_core);
		udelay(5);
		mpp_safe_unreset(enc->rst_a);
		mpp_safe_unreset(enc->rst_h);
		mpp_safe_unreset(enc->rst_core);
		rockchip_pmu_idle_request(mpp->dev, false);
	}

	mpp_debug_leave();

	return 0;
}

static int rkvenc_clk_on(struct mpp_dev *mpp)
{
	struct rkvenc_dev *enc = to_rkvenc_dev(mpp);

	mpp_clk_safe_enable(enc->aclk_info.clk);
	mpp_clk_safe_enable(enc->hclk_info.clk);
	mpp_clk_safe_enable(enc->core_clk_info.clk);

	return 0;
}

static int rkvenc_clk_off(struct mpp_dev *mpp)
{
	struct rkvenc_dev *enc = to_rkvenc_dev(mpp);

	clk_disable_unprepare(enc->aclk_info.clk);
	clk_disable_unprepare(enc->hclk_info.clk);
	clk_disable_unprepare(enc->core_clk_info.clk);

	return 0;
}

static int rkvenc_set_freq(struct mpp_dev *mpp, struct mpp_task *mpp_task)
{
	struct rkvenc_dev *enc = to_rkvenc_dev(mpp);
	struct rkvenc_task *task = to_rkvenc_task(mpp_task);

	mpp_clk_set_rate(&enc->aclk_info, task->clk_mode);
	mpp_clk_set_rate(&enc->core_clk_info, task->clk_mode);

	return 0;
}

static struct mpp_hw_ops rkvenc_hw_ops = {
	.init = rkvenc_init,
	.clk_on = rkvenc_clk_on,
	.clk_off = rkvenc_clk_off,
	.set_freq = rkvenc_set_freq,
	.reset = rkvenc_reset,
};

static struct mpp_dev_ops rkvenc_dev_ops_v2 = {
	.alloc_task = rkvenc_alloc_task,
	.run = rkvenc_run,
	.irq = rkvenc_irq,
	.isr = rkvenc_isr,
	.finish = rkvenc_finish,
	.result = rkvenc_result,
	.free_task = rkvenc_free_task,
	.ioctl = rkvenc_control,
	.init_session = rkvenc_init_session,
	.free_session = rkvenc_free_session,
	.dump_session = rkvenc_dump_session,
};


static const struct mpp_dev_var rkvenc_v2_data = {
	.device_type = MPP_DEVICE_RKVENC,
	.hw_info = &rkvenc_v2_hw_info.hw,
	.trans_info = trans_rkvenc_v2,
	.hw_ops = &rkvenc_hw_ops,
	.dev_ops = &rkvenc_dev_ops_v2,
};

static const struct of_device_id mpp_rkvenc_dt_match[] = {
	{
		.compatible = "rockchip,rkv-encoder-v2",
		.data = &rkvenc_v2_data,
	},
	{},
};

static int rkvenc2_alloc_rcbbuf(struct platform_device *pdev, struct rkvenc_dev *enc)
{
	int ret;
	u32 vals[2];
	dma_addr_t iova;
	u32 sram_used, sram_size;
	struct device_node *sram_np;
	struct resource sram_res;
	resource_size_t sram_start, sram_end;
	struct iommu_domain *domain;
	struct device *dev = &pdev->dev;

	/* get rcb iova start and size */
	ret = device_property_read_u32_array(dev, "rockchip,rcb-iova", vals, 2);
	if (ret) {
		dev_err(dev, "could not find property rcb-iova\n");
		return ret;
	}
	iova = PAGE_ALIGN(vals[0]);
	sram_used = PAGE_ALIGN(vals[1]);
	if (!sram_used) {
		dev_err(dev, "sram rcb invalid.\n");
		return -EINVAL;
	}
	/* alloc reserve iova for rcb */
	ret = iommu_dma_reserve_iova(dev, iova, sram_used);
	if (ret) {
		dev_err(dev, "alloc rcb iova error.\n");
		return ret;
	}
	/* get sram device node */
	sram_np = of_parse_phandle(dev->of_node, "rockchip,sram", 0);
	if (!sram_np) {
		dev_err(dev, "could not find phandle sram\n");
		return -ENODEV;
	}
	/* get sram start and size */
	ret = of_address_to_resource(sram_np, 0, &sram_res);
	of_node_put(sram_np);
	if (ret) {
		dev_err(dev, "find sram res error\n");
		return ret;
	}
	/* check sram start and size is PAGE_SIZE align */
	sram_start = round_up(sram_res.start, PAGE_SIZE);
	sram_end = round_down(sram_res.start + resource_size(&sram_res), PAGE_SIZE);
	if (sram_end <= sram_start) {
		dev_err(dev, "no available sram, phy_start %pa, phy_end %pa\n",
			&sram_start, &sram_end);
		return -ENOMEM;
	}
	sram_size = sram_end - sram_start;
	sram_size = sram_used < sram_size ? sram_used : sram_size;
	/* iova map to sram */
	domain = enc->mpp.iommu_info->domain;
	ret = iommu_map(domain, iova, sram_start, sram_size, IOMMU_READ | IOMMU_WRITE);
	if (ret) {
		dev_err(dev, "sram iommu_map error.\n");
		return ret;
	}
	/* alloc dma for the remaining buffer, sram + dma */
	if (sram_size < sram_used) {
		struct page *page;
		size_t page_size = PAGE_ALIGN(sram_used - sram_size);

		page = alloc_pages(GFP_KERNEL | __GFP_ZERO, get_order(page_size));
		if (!page) {
			dev_err(dev, "unable to allocate pages\n");
			ret = -ENOMEM;
			goto err_sram_map;
		}
		/* iova map to dma */
		ret = iommu_map(domain, iova + sram_size, page_to_phys(page),
				page_size, IOMMU_READ | IOMMU_WRITE);
		if (ret) {
			dev_err(dev, "page iommu_map error.\n");
			__free_pages(page, get_order(page_size));
			goto err_sram_map;
		}
		enc->rcb_page = page;
	}

	enc->sram_size = sram_size;
	enc->sram_used = sram_used;
	enc->sram_iova = iova;
	enc->sram_enabled = -1;
	dev_info(dev, "sram_start %pa\n", &sram_start);
	dev_info(dev, "sram_iova %pad\n", &enc->sram_iova);
	dev_info(dev, "sram_size %u\n", enc->sram_size);
	dev_info(dev, "sram_used %u\n", enc->sram_used);

	return 0;

err_sram_map:
	iommu_unmap(domain, iova, sram_size);

	return ret;
}

static int rkvenc_probe_default(struct platform_device *pdev)
{
	int ret = 0;
	struct device *dev = &pdev->dev;
	struct rkvenc_dev *enc = NULL;
	struct mpp_dev *mpp = NULL;
	const struct of_device_id *match = NULL;

	enc = devm_kzalloc(dev, sizeof(*enc), GFP_KERNEL);
	if (!enc)
		return -ENOMEM;

	mpp = &enc->mpp;
	platform_set_drvdata(pdev, enc);

	if (pdev->dev.of_node) {
		match = of_match_node(mpp_rkvenc_dt_match, pdev->dev.of_node);
		if (match)
			mpp->var = (struct mpp_dev_var *)match->data;
	}

	ret = mpp_dev_probe(mpp, pdev);
	if (ret)
		return ret;

	rkvenc2_alloc_rcbbuf(pdev, enc);

	ret = devm_request_threaded_irq(dev, mpp->irq,
					mpp_dev_irq,
					mpp_dev_isr_sched,
					IRQF_SHARED,
					dev_name(dev), mpp);
	if (ret) {
		dev_err(dev, "register interrupter runtime failed\n");
		goto failed_get_irq;
	}
	mpp->session_max_buffers = RKVENC_SESSION_MAX_BUFFERS;
	enc->hw_info = to_rkvenc_info(mpp->var->hw_info);
	rkvenc_procfs_init(mpp);
	mpp_dev_register_srv(mpp, mpp->srv);

	return 0;

failed_get_irq:
	mpp_dev_remove(mpp);

	return ret;
}

static int rkvenc_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct device *dev = &pdev->dev;

	dev_info(dev, "probing start\n");

	ret = rkvenc_probe_default(pdev);

	dev_info(dev, "probing finish\n");

	return ret;
}

static int rkvenc2_free_rcbbuf(struct platform_device *pdev, struct rkvenc_dev *enc)
{
	struct iommu_domain *domain;

	if (enc->rcb_page) {
		size_t page_size = PAGE_ALIGN(enc->sram_used - enc->sram_size);

		__free_pages(enc->rcb_page, get_order(page_size));
	}
	if (enc->sram_iova) {
		domain = enc->mpp.iommu_info->domain;
		iommu_unmap(domain, enc->sram_iova, enc->sram_used);
	}

	return 0;
}

static int rkvenc_remove(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;

	struct rkvenc_dev *enc = platform_get_drvdata(pdev);

	dev_info(dev, "remove device\n");
	rkvenc2_free_rcbbuf(pdev, enc);
	mpp_dev_remove(&enc->mpp);
	rkvenc_procfs_remove(&enc->mpp);

	return 0;
}

static void rkvenc_shutdown(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	int ret;
	int val;
	struct rkvenc_dev *enc = platform_get_drvdata(pdev);
	struct mpp_dev *mpp = &enc->mpp;

	dev_info(dev, "shutdown device\n");

	if (mpp->srv)
		atomic_inc(&mpp->srv->shutdown_request);

	ret = readx_poll_timeout(atomic_read,
					&mpp->task_count,
					val, val == 0, 1000, 200000);
	if (ret == -ETIMEDOUT)
		dev_err(dev, "wait total running time out\n");

	dev_info(dev, "shutdown success\n");
}

struct platform_driver rockchip_rkvenc2_driver = {
	.probe = rkvenc_probe,
	.remove = rkvenc_remove,
	.shutdown = rkvenc_shutdown,
	.driver = {
		.name = RKVENC_DRIVER_NAME,
		.of_match_table = of_match_ptr(mpp_rkvenc_dt_match),
	},
};
