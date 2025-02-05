package com.painnick.bb8;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
import android.os.Process;
import android.util.Log;

import com.google.mediapipe.framework.MediaPipeException;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;

/**
 * The video player component that reads video frames from the video uri and produces MediaPipe
 */
public class BB8Api {

    private static final String TAG = "BB8Api";

    private final SingleThreadHandlerExecutor executor;
    private WebImageConsumer newBitmapListener;
    private BB8ApiConsumer apiListener;

    private final String hostUrl;
    private final URL imageUrl;

    public BB8Api(String url) {
        hostUrl = url;
        imageUrl = stringToURL(url + "/capture");
        executor = new SingleThreadHandlerExecutor(TAG + "Thread", Process.THREAD_PRIORITY_DEFAULT);
    }

    // Function to convert string to URL
    private URL stringToURL(String string) {
        try {
            return new URL(string);
        } catch (MalformedURLException e) {
            e.printStackTrace();
        }
        return null;
    }

    public void setNewBitmapListener(WebImageConsumer listener) {
        newBitmapListener = listener;
    }

    public void setApiListener(BB8ApiConsumer listener) {
        apiListener = listener;
    }

    public void start() {
        if (newBitmapListener == null) {
            throw new MediaPipeException(
                    MediaPipeException.StatusCode.FAILED_PRECONDITION.ordinal(),
                    "newFrameListener is not set.");
        }
        executor.execute(this::loadImage);
    }

    // Function to establish connection and load image
    private void loadImage() {
        HttpURLConnection connection;
        Bitmap result = null;
        try {
            connection = (HttpURLConnection) imageUrl.openConnection();
            connection.connect();
            InputStream inputStream = connection.getInputStream();
            BufferedInputStream bufferedInputStream = new BufferedInputStream(inputStream);
            result = BitmapFactory.decodeStream(bufferedInputStream);
            connection.disconnect();
        } catch (IOException e) {
            e.printStackTrace();
        }
        if (result != null) {
            this.newBitmapListener.onNewBitmap(result);
        }
    }

    private void call(String url) throws IOException {
        URL commandUrl = stringToURL(hostUrl + url);
        HttpURLConnection connection;
        String jsonString = null;
        try {
            connection = (HttpURLConnection) commandUrl.openConnection();
            connection.connect();
            InputStream inputStream = connection.getInputStream();
            BufferedInputStream bufferedInputStream = new BufferedInputStream(inputStream);
            byte[] contents = new byte[4096];
            int readCount = bufferedInputStream.read(contents);
            if (readCount > 0) {
                jsonString = new String(contents, 0, readCount);
            }
            connection.disconnect();
        } catch (IOException e) {
            e.printStackTrace();
        }
        if (jsonString != null && apiListener != null) {
            JSONObject json = null;
            try {
                json = new JSONObject(jsonString);
            } catch (JSONException e) {
                e.printStackTrace();
            }
            this.apiListener.onResponse(json);
        }
    }

    public void reloadImage() {
        executor.execute(this::loadImage);
    }

    public void moveLeft(boolean found) {
        executor.execute(() -> {
            try {
                call("/motor?dir=left&found=" + (found ? "true" : "false"));
                Log.d(TAG, "moveLeft");
            } catch (IOException e) {
                e.printStackTrace();
            }
            stopNow(found);
        });
    }

    public void moveRight(boolean found) {
        executor.execute(() -> {
            try {
                call("/motor?dir=right&found=" + (found ? "true" : "false"));
                Log.d(TAG, "moveRight");
            } catch (IOException e) {
                e.printStackTrace();
            }
            stopNow(found);
        });
    }

    public void stopNow(boolean found) {
        executor.execute(() -> {
            try {
                call("/motor?dir=none&found=" + (found ? "true" : "false"));
                Log.d(TAG, "stopNow");
            } catch (IOException e) {
                e.printStackTrace();
            }
        });
    }

    /**
     * Closes ImageInput and releases the resources.
     */
    public void close() {
        executor.execute(
                () -> {
                });
    }

    private Bitmap rotateBitmap(Bitmap inputBitmap, int degree) throws IOException {
        Matrix matrix = new Matrix();
        matrix.postRotate(degree);
        return Bitmap.createBitmap(inputBitmap, 0, 0, inputBitmap.getWidth(), inputBitmap.getHeight(), matrix, true);
    }

}
