package com.rockchip.gpadc.demo;




/**
 * Created by randall on 18-4-27.
 */


/*
* 一个简单的bufferqueue实现
*
* */
public class ImageBufferQueue {
    private ImageBuffer[] mQueueBuffer;
    private int mQueueBufferSize;
    private int mCurrentFreeBufferIndex;
    private int mCurrentUsingBufferIndex;

    public ImageBufferQueue(int bufferSize, int width, int height) {
        mCurrentFreeBufferIndex = -1;
        mCurrentUsingBufferIndex = -1;
        mQueueBufferSize = bufferSize;
        mQueueBuffer = new ImageBuffer[mQueueBufferSize];

        for (int i=0; i<mQueueBufferSize; ++i) {
            mQueueBuffer[i] = new ImageBuffer(width, height);
        }
    }

    public void release() {
        for (int i=0; i<mQueueBufferSize; ++i) {
            mQueueBuffer[i] = null;
        }

        mQueueBuffer = null;
    }

    //获取待处理的数据
    public synchronized ImageBuffer getReadyBuffer() {

        int index = mCurrentUsingBufferIndex;

        for (int i=0; i<mQueueBufferSize; ++i) {
            ++index;

            if (index >= mQueueBufferSize) {
                index = 0;
            }

            if (mQueueBuffer[index].mStatus == ImageBuffer.STATUS_READY) {
                break;
            }
        }

        if ((index != mCurrentUsingBufferIndex) && (mQueueBuffer[index].mStatus == ImageBuffer.STATUS_READY)) {
            mCurrentUsingBufferIndex = index;
            mQueueBuffer[index].mStatus = ImageBuffer.STATUS_USING;

            return mQueueBuffer[index];
        }

        return null;
    }

    public synchronized void releaseBuffer(ImageBuffer buffer) {
        buffer.mStatus = ImageBuffer.STATUS_INVAILD;
    }

    public  synchronized ImageBuffer getFreeBuffer() {

        int index = mCurrentFreeBufferIndex;

        for (int i=0; i<mQueueBufferSize; ++i) {
            ++index;

            if (index >= mQueueBufferSize) {
                index = 0;
            }

            if (mQueueBuffer[index].mStatus != ImageBuffer.STATUS_USING) {
                break;
            }
        }

        mCurrentFreeBufferIndex = index;

        mQueueBuffer[index].mStatus = ImageBuffer.STATUS_INVAILD;
        return mQueueBuffer[index];
    }
    public synchronized void postBuffer(ImageBuffer buffer) {
        buffer.mStatus = ImageBuffer.STATUS_READY;
    }

    public class ImageBuffer {
        static public final int STATUS_INVAILD = 0;
        static public final int STATUS_READY = 1;
        static public final int STATUS_USING = 2;


        public int mStatus;
        public int mWidth;
        public int mHeight;
        public int mBufferSize; //nv21
        public byte[] mImage;


        public ImageBuffer(int width, int height) {
            mStatus = STATUS_INVAILD;
            mWidth = width;
            mHeight = height;
            mBufferSize = mWidth * mHeight * 4;
            mImage = new byte[mBufferSize];

        }

        public void finalize() {

        }

    }
}
