/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2005  Wengo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef OWWEBCAMDRIVER_H
#define OWWEBCAMDRIVER_H

#include <webcam/IWebcamDriver.h>

#include <pixertool/pixertool.h>

#include <cutil/global.h>

#include <thread/RecursiveMutex.h>

#if defined(OS_WIN32)
	class DirectXWebcamDriver;
#elif defined(OS_MACOSX)
	class QuicktimeWebcamDriver;
#elif defined(OS_LINUX)
	class V4LWebcamDriver;
#endif

class WebcamDriverFactory;

/**
 * TODO: implements a setFlags method for WEBCAM_FORCE_PALETTE_CONVERSION
 *
 * @see IWebcamDriver
 * @author Philippe Bernery
 * @author Mathieu Stute
 */
class WebcamDriver : public IWebcamDriver {
public:

	/**
	 * Initialize the api.
	 * Under Windows any thread accessing this api must call
	 * this function in order to use it.
	 */
	OWWEBCAM_API static void apiInitialize();

	/**
	 * Uninitialize the api.
	 * Under Windows any thread that has called apiInitialize 
	 * must call this function.
	 */
	OWWEBCAM_API static void apiUninitialize();

	/**
	 * Sets current factory
	 *
	 * @param factory factory to use
	 */
	OWWEBCAM_API static void setFactory(WebcamDriverFactory * factory);

	/**
	 * Gets instance of WebcamDriver.
	 *
	 * Implements singleton pattern. This method create a WebcamDriver with
	 * WEBCAM_FORCE_IMAGE_FORMAT flag.
	 */
	OWWEBCAM_API static WebcamDriver * getInstance();

	OWWEBCAM_API void setFlags(int flags);

	OWWEBCAM_API void unsetFlags(int flags);

	OWWEBCAM_API bool isFlagSet(int flag);

	OWWEBCAM_API StringList getDeviceList();

	OWWEBCAM_API std::string getDefaultDevice();

	OWWEBCAM_API WebcamErrorCode setDevice(const std::string & deviceName);

	OWWEBCAM_API bool isOpen() const;

	OWWEBCAM_API void startCapture();

	OWWEBCAM_API void pauseCapture();

	/**
	 * Stops webcam capture.
	 *
	 * There must be same amount of call to stopCapture than to
	 * startCapture to uninitialize correctly the driver.
	 */
	OWWEBCAM_API void stopCapture();

	OWWEBCAM_API WebcamErrorCode setPalette(pixosi palette);

	OWWEBCAM_API pixosi getPalette() const;

	OWWEBCAM_API WebcamErrorCode setFPS(unsigned fps);

	OWWEBCAM_API unsigned getFPS() const;

	OWWEBCAM_API WebcamErrorCode setResolution(unsigned width, unsigned height);

	OWWEBCAM_API unsigned getWidth() const;

	OWWEBCAM_API unsigned getHeight() const;

	OWWEBCAM_API void setBrightness(int brightness);

	OWWEBCAM_API int getBrightness() const;

	OWWEBCAM_API void setContrast(int contrast);

	OWWEBCAM_API int getContrast() const;

	OWWEBCAM_API void flipHorizontally(bool flip);

	/** Do not use: this is an internal method. */
	void cleanup();

private:

	static WebcamDriver * instance;

	WebcamDriver(int flags);

	virtual ~WebcamDriver();

#if defined(OS_WINDOWS)
	friend class DirectXWebcamDriver;
#elif defined(OS_MACOSX)
	friend class QuicktimeWebcamDriver;
#elif defined(OS_LINUX)
	friend class V4LWebcamDriver;
#endif

	/**
	 * Called by platform driver when a frame is available.
	 *
	 * The frame will then be sent if the time since last frame correponds
	 * to desired fps. The fps can be managed by the software if the webcam
	 * does not support the requested fps
	 * @param image captured image
	 */
	void frameBufferAvailable(piximage * image);

	/**
	 * @see WEBCAM_FORCE_IMAGE_FORMAT
	 */
	bool isFormatForced() const;

	/**
	 * Initializes _convImage.
	 */
	void initializeConvImage();

	/**
	 * Forces capture to stop.
	 */
	void forceStopCapture();

	/** Pointer to factory to use. */
	static WebcamDriverFactory * _factory;

	/** Pointer to paltform dependent webcam driver. */
	IWebcamDriver * _webcamPrivate;

	/** Desired FPS. */
	unsigned _forcedFPS;

	/** Time since last frame. */
	float _fpsTimerLast;

	/**
	 * True if FPS must be forced.
	 */
	bool _forceFPS;

	/** Desired palette. */
	pixosi _desiredPalette;

	/** Desired width. */
	unsigned _desiredWidth;

	/** Desired height */
	unsigned _desiredHeight;

	/** Flags. */
	int _flags;

	/** Contains the converted captured frame. */
	piximage * _convImage;

	/** Conversion flags. */
	int _convFlags;

	/** Call to startCapture counter. */
	unsigned _startCounter;

	/** Mutex for thread safety. */
	mutable RecursiveMutex _mutex;
};

#endif	//OWWEBCAMDRIVER_H
