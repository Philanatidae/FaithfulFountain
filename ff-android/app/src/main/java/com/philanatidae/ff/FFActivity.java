/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

package com.philanatidae.ff;

import android.app.NativeActivity;
import android.content.Context;
import android.content.Intent;
import android.graphics.Rect;
import android.media.AudioManager;
import android.net.Uri;
import android.os.Build;
import android.view.Display;
import android.view.DisplayCutout;
import android.view.WindowManager;

import androidx.core.view.ViewCompat;
import androidx.core.view.WindowCompat;
import androidx.core.view.WindowInsetsCompat;
import androidx.core.view.WindowInsetsControllerCompat;

import java.util.concurrent.ExecutionException;
import java.util.concurrent.FutureTask;
import java.util.concurrent.RunnableFuture;

public class FFActivity extends NativeActivity {
    /** FF HELPERS **/
    public int getAudioSampleRate() {
        AudioManager audioManager = (AudioManager)getSystemService(Context.AUDIO_SERVICE);
        String sampleRateStr = audioManager.getProperty(AudioManager.PROPERTY_OUTPUT_SAMPLE_RATE);
        return Integer.parseInt(sampleRateStr);
    }
    public int getAudioFramesPerBuffer() {
        AudioManager audioManager = (AudioManager)getSystemService(Context.AUDIO_SERVICE);
        String framesPerBufferStr = audioManager.getProperty(AudioManager.PROPERTY_OUTPUT_FRAMES_PER_BUFFER);
        return Integer.parseInt(framesPerBufferStr);
    }

    public float getTargetFrameRate() {
        Display display;
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.R) {
            display = getDisplay();
        } else {
            display = getWindowManager().getDefaultDisplay();
        }
        float refreshRate = display.getRefreshRate();
        // Round to nearest hundredth
        refreshRate *= 100.0f;
        refreshRate = Math.round(refreshRate);
        refreshRate /= 100.0f;
        return refreshRate;
    }

    public void openURL(String url) {
        Intent view = new Intent(Intent.ACTION_VIEW, Uri.parse(url));
        startActivity(view);
    }

    public void setToImmersiveMode() {
        Runnable runnable = new Runnable() {
            @Override
            public void run() {
                // Fullscreen
                getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);

                // Edge-to-edge mode
                WindowCompat.setDecorFitsSystemWindows(getWindow(), false);
                if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
                    getWindow().getAttributes().layoutInDisplayCutoutMode = WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES;
                    getWindow().setAttributes(getWindow().getAttributes());
                }

                // Immersive mode
                WindowInsetsControllerCompat windowInsetsController = ViewCompat.getWindowInsetsController(getWindow().getDecorView());
                if(windowInsetsController != null) {
                    // Hide system bars
                    windowInsetsController.setSystemBarsBehavior(WindowInsetsControllerCompat.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE);
                    windowInsetsController.hide(WindowInsetsCompat.Type.systemBars());
                }

                synchronized (this) {
                    this.notify();
                }
            }
        };
        
        RunnableFuture<Void> task = new FutureTask<>(runnable, null);
        runOnUiThread(task);
        // Wait on this thread until the window has been updated on the UI
        // thread. Otherwise we can potentially have a race condition.
        // https://stackoverflow.com/questions/34459392/java-wait-for-runnable-to-finish
        try {
            task.get();
        } catch (InterruptedException e) {
            e.printStackTrace();
        } catch(ExecutionException e) {
            e.printStackTrace();
        }
    }
    public Rect getSafeArea() {
        // Get dimensions the window we are in
        Rect rect = new Rect(0, 0,
                getWindow().getDecorView().getWidth(),
                getWindow().getDecorView().getHeight());

        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.Q
            && getWindowManager().getDefaultDisplay() != null) {
            // @todo `getDefaultDisplay` is deprecated in API level 30. Replace with `Context#getDisplay`
            DisplayCutout cutout = getWindowManager().getDefaultDisplay().getCutout();
            if(cutout != null) {
                // Get the 1) window position and 2) screen size
                Rect visibleDisplayFrame = new Rect();
                getWindow().getDecorView().getWindowVisibleDisplayFrame(visibleDisplayFrame);

                int screenWidth = visibleDisplayFrame.left + visibleDisplayFrame.right;
                int screenHeight = visibleDisplayFrame.top + visibleDisplayFrame.bottom;

                int safeLeft = cutout.getSafeInsetLeft();
                int safeRight = cutout.getSafeInsetRight();
                int safeTop = cutout.getSafeInsetTop();
                int safeBottom = cutout.getSafeInsetBottom();

                // If any part of the window is in the cutout, we need to
                // modify the safe area to account for this.
                Rect safeRect = new Rect(rect);

                if(visibleDisplayFrame.left < safeLeft) {
                    safeRect.left += safeLeft - visibleDisplayFrame.left;
                }
                if(screenWidth - (visibleDisplayFrame.left + rect.right)
                    < safeRight) {
                    safeRect.right -= safeRight;
                }
                if(visibleDisplayFrame.top < safeTop) {
                    safeRect.top += safeTop - visibleDisplayFrame.top;
                }
            if(screenHeight - (visibleDisplayFrame.top + rect.bottom)
                    < safeBottom) {
                    safeRect.bottom -= safeBottom;
                }

                rect = safeRect;
            }
        }
        return rect;
    }

    String[] getCLIOptions() {
        Intent intent = getIntent();
        String clioptions = intent.getStringExtra("clioptions");
        String[] options = clioptions.split("(?<!\")\\s+(?!\")");
        return options;
    }
}
