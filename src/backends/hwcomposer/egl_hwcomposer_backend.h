/*
    KWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2015 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef KWIN_EGL_HWCOMPOSER_BACKEND_H
#define KWIN_EGL_HWCOMPOSER_BACKEND_H
#include "abstract_egl_backend.h"
#include "utils/damagejournal.h"
#include <KF5/KWayland/Server/outputdevice_interface.h>

#define ROTATE_EGL 0
namespace KWin
{

class HwcomposerBackend;
class HwcomposerWindow;
class HwcomposerOutput;
class EglHwcomposerBackend : public AbstractEglBackend
{
public:
    EglHwcomposerBackend(HwcomposerBackend *backend);
    virtual ~EglHwcomposerBackend();
    std::unique_ptr<SurfaceTexture> createSurfaceTextureInternal(SurfacePixmapInternal *pixmap) override;
    std::unique_ptr<SurfaceTexture> createSurfaceTextureWayland(SurfacePixmapWayland *pixmap) override;
    QRegion beginFrame(Output *output);
    void endFrame(Output *output, const QRegion &renderedRegion, const QRegion &damagedRegion);
    void init() override;

private:
    bool initializeEgl();
    bool initRenderingContext();
    bool initBufferConfigs() override;
    bool makeContextCurrent();
    HwcomposerBackend *m_backend;
    HwcomposerWindow *m_nativeSurface = nullptr;
    DamageJournal m_damageJournal;
};

}

#endif
