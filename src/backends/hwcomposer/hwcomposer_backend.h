/*
    KWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2015 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef KWIN_HWCOMPOSER_BACKEND_H
#define KWIN_HWCOMPOSER_BACKEND_H
#include "core/outputbackend.h"
#include "core/output.h"
#include "input.h"
#include "backends/libinput/libinputbackend.h"

#include <QElapsedTimer>
#include <QMutex>
#include <QWaitCondition>
#include <QSemaphore>
#include <QFile>
#include <QTimer>

// libhybris
#include <android/hardware/hwcomposer.h>
#include <hybris/hwcomposerwindow/hwcomposer_window.h>
#include <hybris/hwc2/hwc2_compatibility_layer.h>
#include <hybris/hwcomposerwindow/hwcomposer.h>
#include <QBasicTimer>

// needed as hwcomposer_window.h includes EGL which on non-arm includes Xlib
#include <fixx11h.h>
#include "core/renderloop.h"
#include <KF5/KWayland/Server/output_interface.h>

typedef struct hwc_display_contents_1 hwc_display_contents_1_t;
typedef struct hwc_layer_1 hwc_layer_1_t;
typedef struct hwc_composer_device_1 hwc_composer_device_1_t;
struct light_device_t;

class HWComposerNativeWindowBuffer;

namespace KWin
{

class HwcomposerWindow;
class HwcomposerBackend;
class BacklightInputEventFilter;


class HwcomposerOutput : public Output
{
    Q_OBJECT
public:
    HwcomposerOutput(HwcomposerBackend *backend, hwc2_compat_display_t* hwc2_primary_display);
    ~HwcomposerOutput() override;

    void init();

    RenderLoop *renderLoop() const override;
    bool isValid() const;
    bool hardwareTransforms() const;
    void setDpmsMode(DpmsMode mode) override;
    void setEnabled(bool enable);
    bool isEnabled() const;
Q_SIGNALS:
    void dpmsModeRequested(HwcomposerOutput::DpmsMode mode);
private:
    QSize m_pixelSize;
    hwc2_compat_display_t *m_hwc2_primary_display;
    friend class HwcomposerBackend;
    RenderLoop *m_renderLoop;
    HwcomposerBackend *m_backend;
    bool m_isEnabled = true;
};

class HwcomposerBackend : public OutputBackend
{
    Q_OBJECT

public:
    explicit HwcomposerBackend(QObject *parent = nullptr);
    virtual ~HwcomposerBackend();

    bool initialize() override;
/*    OpenGLBackend *createOpenGLBackend() override; */
    std::unique_ptr<OpenGLBackend> createOpenGLBackend() override;


    Outputs outputs() const override;
    Outputs enabledOutputs() const;

    QSize size() const;
    QSize screenSize() const;

    int scale() const;
    Session *session() const;

    HwcomposerWindow *createSurface();

    std::unique_ptr<InputBackend> createInputBackend() override;
/*    InputBackend *createInputBackend() override; */

    void enableVSync(bool enable);
    void waitVSync();
    void wakeVSync();
    QVector<CompositingType> supportedCompositors() const override {
        return QVector<CompositingType>{OpenGLCompositing};
    }

    bool isBacklightOff() const {
        return m_outputBlank;
    }

    hwc2_compat_device_t *hwc2_device() const {
        return m_hwc2device;
    }

    hwc2_compat_display_t *hwc2_display() const {
        return m_hwc2_primary_display;
    }

Q_SIGNALS:
   void outputBlankChanged();

private Q_SLOTS:
    void toggleBlankOutput();
    void screenBrightnessChanged(int brightness) {
        m_oldScreenBrightness = brightness;
    }

    void compositing(int flags);

private:
    friend HwcomposerWindow;
    void setPowerMode(bool enable);
    bool updateOutputs();
    void updateOutputsEnabled();
    void toggleScreenBrightness();
    void initLights();

    light_device_t *m_lights = nullptr;
    int m_vsyncInterval = 16;
    bool m_hasVsync = false;
    QMutex m_vsyncMutex;
    QWaitCondition m_vsyncWaitCondition;
    QSemaphore     m_compositingSemaphore;
    QScopedPointer<BacklightInputEventFilter> m_filter;
    QScopedPointer<HwcomposerOutput> m_output;
    bool m_outputBlank = true;
    int m_oldScreenBrightness = 0x7f;

    void RegisterCallbacks();

    hwc2_compat_device_t *m_hwc2device = nullptr;
    hwc2_compat_display_t* m_hwc2_primary_display = nullptr;
    Session *m_session = nullptr;
};

class HwcomposerWindow : public HWComposerNativeWindow
{
public:
    virtual ~HwcomposerWindow();
    void present(HWComposerNativeWindowBuffer *buffer) override;

private:
    friend HwcomposerBackend;
    HwcomposerWindow(HwcomposerBackend *backend);
    HwcomposerBackend *m_backend;
    int lastPresentFence = -1;

    hwc2_compat_display_t *m_hwc2_primary_display = nullptr;
};

class BacklightInputEventFilter : public QObject,  public InputEventFilter
{
public:
    BacklightInputEventFilter(HwcomposerBackend *backend);
    virtual ~BacklightInputEventFilter();

    bool pointerEvent(QMouseEvent *event, quint32 nativeButton);
    bool wheelEvent(QWheelEvent *event);
    bool keyEvent(QKeyEvent *event);
    bool touchDown(qint32 id, const QPointF &pos, quint32 time);
    bool touchMotion(qint32 id, const QPointF &pos, quint32 time);
    bool touchUp(qint32 id, quint32 time);
private:
    void toggleBacklight();
    HwcomposerBackend *m_backend;
    QElapsedTimer m_doubleTapTimer;
    QVector<qint32> m_touchPoints;
    bool m_secondTap = false;
};

}

#endif
