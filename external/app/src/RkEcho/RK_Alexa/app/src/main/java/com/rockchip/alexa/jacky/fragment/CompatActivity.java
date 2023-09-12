/*
 * Copyright © Yan Zhenjie. All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.rockchip.alexa.jacky.fragment;

import android.os.Bundle;
import android.support.annotation.IdRes;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentTransaction;
import android.support.v7.app.AppCompatActivity;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * Created by Yan Zhenjie on 2017/1/13.
 */
public abstract class CompatActivity extends AppCompatActivity {

    public static final int REQUEST_CODE_INVALID = -1;

    private AtomicInteger mAtomicInteger = new AtomicInteger();
    private List<NoFragment> mFragmentStack = new ArrayList<>();
    private Map<NoFragment, FragmentStackEntity> mFragmentEntityMap = new HashMap<>();

    static class FragmentStackEntity {
        private FragmentStackEntity() {
        }

        private boolean isSticky = false;
        private int requestCode = REQUEST_CODE_INVALID;
        @ResultCode
        int resultCode = RESULT_CANCELED;
        Bundle result = null;
    }

    /**
     * Show a fragment.
     *
     * @param clazz fragment class.
     */
    public final <T extends NoFragment> void startFragment(Class<T> clazz) {
        try {
            NoFragment targetFragment = clazz.newInstance();
            startFragment(null, targetFragment, true, REQUEST_CODE_INVALID);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public <T extends NoFragment> void startFragmentClearStack(Class<T> cla) {
        if (mFragmentStack != null) {
            mFragmentStack.clear();
        }
        startFragment(cla);
    }

    /**
     * Show a fragment.
     *
     * @param clazz       fragment class.
     * @param stickyStack sticky to back stack.
     */
    public final <T extends NoFragment> void startFragment(Class<T> clazz, boolean stickyStack) {
        try {
            NoFragment targetFragment = clazz.newInstance();
            startFragment(null, targetFragment, stickyStack, REQUEST_CODE_INVALID);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * Show a fragment.
     *
     * @param targetFragment fragment to display.
     * @param <T>            {@link NoFragment}.
     */
    public final <T extends NoFragment> void startFragment(T targetFragment) {
        startFragment(null, targetFragment, true, REQUEST_CODE_INVALID);
    }

    /**
     * Show a fragment.
     *
     * @param targetFragment fragment to display.
     * @param stickyStack    sticky back stack.
     * @param <T>            {@link NoFragment}.
     */
    public final <T extends NoFragment> void startFragment(T targetFragment, boolean stickyStack) {
        startFragment(null, targetFragment, stickyStack, REQUEST_CODE_INVALID);
    }

    /**
     * Show a fragment.
     *
     * @param clazz       fragment to display.
     * @param requestCode requestCode.
     * @param <T>         {@link NoFragment}.
     */
    public <T extends NoFragment> void startFragmentForResquest(Class<T> clazz, int requestCode) {
        if (requestCode == REQUEST_CODE_INVALID)
            throw new IllegalArgumentException("The requestCode must be positive integer.");
        try {
            NoFragment targetFragment = clazz.newInstance();
            startFragment(null, targetFragment, true, requestCode);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * Show a fragment.
     *
     * @param targetFragment fragment to display.
     * @param requestCode    requestCode.
     * @param <T>            {@link NoFragment}.
     */
    public <T extends NoFragment> void startFragmentForResquest(T targetFragment, int requestCode) {
        if (requestCode == REQUEST_CODE_INVALID)
            throw new IllegalArgumentException("The requestCode must be positive integer.");
        startFragment(null, targetFragment, true, requestCode);
    }

    /**
     * Show a fragment.
     *
     * @param nowFragment    Now show fragment, can be null.
     * @param targetFragment fragment to display.
     * @param stickyStack    sticky back stack.
     * @param requestCode    requestCode.
     * @param <T>            {@link NoFragment}.
     */
    public <T extends NoFragment> void startFragment(T nowFragment, T targetFragment, boolean stickyStack, int
            requestCode) {
        FragmentStackEntity fragmentStackEntity = new FragmentStackEntity();
        fragmentStackEntity.isSticky = stickyStack;
        fragmentStackEntity.requestCode = requestCode;
        targetFragment.setStackEntity(fragmentStackEntity);

        FragmentManager fragmentManager = getSupportFragmentManager();
        FragmentTransaction fragmentTransaction = fragmentManager.beginTransaction();
        if (nowFragment != null) {
            if (mFragmentEntityMap.get(nowFragment).isSticky) {
                nowFragment.onPause();
                nowFragment.onStop();
                fragmentTransaction.hide(nowFragment);
            } else {
                fragmentTransaction.remove(nowFragment);
                fragmentTransaction.commit();
                mFragmentStack.remove(nowFragment);
                mFragmentEntityMap.remove(nowFragment);

                fragmentTransaction = fragmentManager.beginTransaction();
            }
        }

        String fragmentTag = targetFragment.getClass().getSimpleName() + mAtomicInteger.incrementAndGet();
        fragmentTransaction.add(fragmentLayoutId(), targetFragment, fragmentTag);
        fragmentTransaction.addToBackStack(fragmentTag);
        fragmentTransaction.commitAllowingStateLoss();

        mFragmentStack.add(targetFragment);
        mFragmentEntityMap.put(targetFragment, fragmentStackEntity);
    }

    /**
     * When the back off.
     */
    private boolean onBackStackFragment() {
        FragmentManager fragmentManager = getSupportFragmentManager();
        if (mFragmentStack.size() > 1) {
            fragmentManager.popBackStack();
            NoFragment inFragment = mFragmentStack.get(mFragmentStack.size() - 2);

            FragmentTransaction fragmentTransaction = fragmentManager.beginTransaction();
            fragmentTransaction.show(inFragment);
            fragmentTransaction.commit();

            NoFragment outFragment = mFragmentStack.get(mFragmentStack.size() - 1);
            inFragment.onResume();

            FragmentStackEntity fragmentStackEntity = mFragmentEntityMap.get(outFragment);
            mFragmentStack.remove(outFragment);
            mFragmentEntityMap.remove(outFragment);

            if (fragmentStackEntity.requestCode != REQUEST_CODE_INVALID) {
                inFragment.onFragmentResult(fragmentStackEntity.requestCode, fragmentStackEntity.resultCode,
                        fragmentStackEntity.result);
            }
            return true;
        }
        return false;
    }

    @Override
    public void onBackPressed() {
        if (!onBackStackFragment()) {
            finish();
        }
    }

    /**
     * Should be returned to display fragments id of {@link android.view.ViewGroup}.
     *
     * @return resource id of {@link android.view.ViewGroup}.
     */
    protected abstract
    @IdRes
    int fragmentLayoutId();

}