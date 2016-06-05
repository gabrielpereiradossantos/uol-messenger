/*
 * C/Python wrapper for phapi
 *
 * Copyright (C) 2005-2007 Wengo SAS
 * Copyright (C) 2005 David Ferlier <david.ferlier@wengo.fr>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2,
 * or (at your option) any later version.
 *
 * This is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with dpkg; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

/**
 * @author David Ferlier <david.ferlier@wengo.fr>
 * @author Mathieu Stute <mathieu.stute@wengo.com>
 */

/**
 *  TODO: debug video support
 *
 *  - conference functions
 */

#define MAX_URI_LENGTH 512
#define MAX_IP_LENGTH 64
#define MAX_PASSWORD_LENGTH 64
#define MAX_USERNAME_LENGTH 64

#include <Python.h>
#include <phapi.h>

#ifdef WIN32
    #include <winsock2.h>
    #undef DEBUG
    #undef _DEBUG

    #include <windows.h>
    #include <shlwapi.h>

    static void (*userNotify)(char *buf, int size);
    LONG unhandledExceptionFilter(struct _EXCEPTION_POINTERS * pExceptionInfo) {
        userNotify(NULL, NULL);
        return 0;
    }
#endif  //OS_WINDOWS

static void pyphapi_callback_callProgress(int cid,
                const phCallStateInfo_t *info);
static void pyphapi_callback_registerProgress(int cid,
                int regStatus);
static void pyphapi_callback_transferProgress(int cid,
                const phTransferStateInfo_t *info);
static void pyphapi_callback_confProgress(int cfid,
                const phConfStateInfo_t *info);
static void pyphapi_callback_msgProgress(int mid,
                const phMsgStateInfo_t *info);
static void pyphapi_callback_onNotify(const char* event,
                const char* from, const char* content);
static void pyphapi_callback_subscriptionProgress(int sid,
                const phSubscriptionStateInfo_t *info);
static void pyphapi_callback_errorNotify(enum phErrors error);
static void pyphapi_callback_frameDisplay(int cid,
                phVideoFrameReceivedEvent_t *ev);

static phCallbacks_t pyphapi_callbacks = {
    pyphapi_callback_callProgress,
    pyphapi_callback_transferProgress,
    pyphapi_callback_confProgress,
    pyphapi_callback_registerProgress,
    pyphapi_callback_msgProgress,
    pyphapi_callback_onNotify,
    pyphapi_callback_subscriptionProgress,
    pyphapi_callback_frameDisplay,
    pyphapi_callback_errorNotify,
    NULL,
};

static PyObject *pyphapi_callProgress = NULL;
static PyObject *pyphapi_registerProgress =  NULL;
static PyObject *pyphapi_subscriptionProgress =  NULL;
static PyObject *pyphapi_confProgress =  NULL;
static PyObject *pyphapi_onNotify = NULL;
static PyObject *pyphapi_transferProgress = NULL;
static PyObject *pyphapi_msgProgress = NULL;
static PyObject *pyphapi_errorNotify = NULL;
static PyObject *pyphapi_frameDisplay = NULL;

void pyphapi_lock_and_call(PyObject *callback, PyObject *args);
PyMODINIT_FUNC initpyphapi(void);

#if 0
/**
 * @brief Initializes the module
 */
static PyObject * pyphapi(PyObject* self) {
    PyEval_InitThreads();

    return Py_BuildValue("s","Hello");
}
#endif

/*****************************************************************************
 *                                   OLD API                                 *
 *****************************************************************************/

/**
 * @brief Wraps phInit()
 */
static PyObject * PyPhInit(PyObject * self, PyObject * params) {

    char * server;
    int asyncmode;
    int ret = -1;

    if (PyArg_ParseTuple(params, "si", &server, &asyncmode)) {
        ret = phInit(&pyphapi_callbacks, server, asyncmode);
    }

    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps phAddAuthInfo()
 */
static PyObject * PyPhAddAuthInfo(PyObject *self, PyObject *params) {
    const char *username;
    const char *userid;
    const char *passwd;
    const char *ha1;
    const char *realm;
    int pycode, ret;

    pycode = PyArg_ParseTuple(params, "sssss", &username,
            &userid, &passwd, &ha1, &realm);

    if (!pycode) {
        return Py_None;
    }

    ret = phAddAuthInfo(username, userid, passwd, ha1, realm);
    return Py_BuildValue("i", ret);
}

/**
 * @brief phAddVline
 */
static PyObject * PyPhAddVline(PyObject *self, PyObject *params) {
    const char *username;
    const char *host;
    const char *proxy;
    int regTimeout;
    int pycode, ret;

    pycode = PyArg_ParseTuple(params, "sssi", &username,
            &host, &proxy, &regTimeout);

    if (!pycode) {
        return Py_None;
    }

    ret = phAddVline(username, host, proxy, regTimeout);
    return Py_BuildValue("i", ret);
}

/**
 * @brief phTunnelConfig
 */
static PyObject * PyPhTunnelConfig(PyObject *self, PyObject *params) {
    const char *http_proxy;
    const char *httpt_server;
    const char *proxy_user;
    const char *proxy_passwd;
    const int http_proxy_port;
    const int httpt_server_port;
    int autoconf;
    int ret = -1;

    if (PyArg_ParseTuple(params, "sisissi",
                &http_proxy, &http_proxy_port,
                &httpt_server, &httpt_server_port,
                &proxy_user, &proxy_passwd, &autoconf)) {

        ret = phTunnelConfig(http_proxy,
                             http_proxy_port,
                             httpt_server,
                             httpt_server_port,
                             proxy_user,
                             proxy_passwd,
                             0,
                             autoconf);
    }

    return Py_BuildValue("i", ret);
}

/**
 * @brief phAddVline2
 */
static PyObject * PyPhAddVline2(PyObject *self, PyObject *params) {
    const char *displayname;
    const char *username;
    const char *host;
    const char *proxy;
    int regTimeout;
    int pycode, ret;
    phConfig_t *cfg;

    cfg = phGetConfig();

    pycode = PyArg_ParseTuple(params, "ssssi", &displayname, &username,
            &host, &proxy, &regTimeout);

    if (!pycode) {
        return Py_None;
    }

    strcpy(cfg->proxy, proxy);

    ret = phAddVline2(displayname, username, host, proxy, regTimeout);
    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps phPoll()
 */
static PyObject * PyPhPoll(PyObject *self, PyObject *params) {
    int ret;

    ret = phPoll();
    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps phRefresh()
 */
static PyObject * PyPhRefresh(PyObject *self, PyObject *params) {
    phRefresh();
    return Py_None;
}

/**
 * @brief Wraps phTerminate()
 */
static PyObject * PyPhTerminate(PyObject *self, PyObject *params) {
    phTerminate();
    return Py_None;
}

/**
 * @brief Wraps phLinePlaceCall()
 */
static PyObject * PyPhLinePlaceCall(PyObject *self, PyObject *params) {
    const char *uri;
    int rcid;
    void *userdata;
    int vlid;
    int ret = -12;

    if (PyArg_ParseTuple(params, "issi", &vlid, &uri, &userdata, &rcid)) {
        ret = phLinePlaceCall(vlid, uri, userdata, rcid);
    }

    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps phLinePlaceCall2()
 */
static PyObject * PyPhLinePlaceCall2(PyObject *self, PyObject *params) {
    const char *uri;
    int rcid;
    int streams;
    void *userdata;
    int vlid;
    int ret = -12;

    if (PyArg_ParseTuple(params, "issii", &vlid, &uri, &userdata, &rcid, &streams)) {
        ret = phLinePlaceCall2(vlid, uri, userdata, rcid, streams);
    }

    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps phLineSendOptions
 */
static PyObject * PyPhLineSendOptions(PyObject *self, PyObject *params) {
    int vlid;
    const char *to;
    int ret = -1;

    if (PyArg_ParseTuple(params, "is", &vlid, &to)) {
        ret = phLineSendOptions(vlid, to);
    }

    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps phLineSendMessage()
 */
static PyObject * PyPhLineSendMessage(PyObject *self, PyObject *params) {
    int vlid;
    const char *to;
    const char *buff;
    int ret = -1;

    if (PyArg_ParseTuple(params, "iss", &vlid, &to, &buff)) {
        ret = phLineSendMessage(vlid, to, buff, "text/plain");
    }

    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps phLinePublish()
 */
static PyObject * PyPhLinePublish(PyObject *self, PyObject *params) {
    int vlid;
    const int winfo;
    const char *to;
    const char *content_type;
    const char *content;
    int ret = -1;

    if (PyArg_ParseTuple(params, "isiss", &vlid, &to, &winfo, &content_type, &content)) {
        ret = phLinePublish(vlid, to, winfo, content_type, content);
    }

    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps phLineSubscribe
 */
static PyObject * PyPhLineSubscribe(PyObject *self, PyObject *params) {
    int vlid;
    const int winfo;
    const char *to;
    int ret = -1;

    if (PyArg_ParseTuple(params, "isi", &vlid, &to, &winfo)) {
        ret = phLineSubscribe(vlid, to, winfo);
    }

    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps phLineSetFollowMe
 */
static PyObject * PyPhLineSetFollowMe(PyObject *self, PyObject *params) {
    int vlid;
    const char *uri;
    int ret = -1;

    if (PyArg_ParseTuple(params, "is", &vlid, &uri)) {
        ret = phLineSetFollowMe(vlid, uri);
    }

    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps phLineSetBusy
 */
static PyObject * PyPhLineSetBusy(PyObject *self, PyObject *params) {
    int vlid;
    int busy_flag;
    int ret = -1;

    if (PyArg_ParseTuple(params, "is", &vlid, &busy_flag)) {
        ret = phLineSetBusy(vlid, busy_flag);
    }

    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps phAcceptCall2()
 */
static PyObject * PyPhAcceptCall2(PyObject *self, PyObject *params) {
    int cid;
    int ret = -1;

    if (PyArg_ParseTuple(params, "i", &cid)) {
        ret = phAcceptCall2(cid, 0);
    }

    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps phAcceptCall3()
 */
static PyObject * PyPhAcceptCall3(PyObject *self, PyObject *params) {
    int cid;
    int stream;
    int ret = -1;

    if (PyArg_ParseTuple(params, "ii", &cid, &stream)) {
        ret = phAcceptCall3(cid, 0, stream);
    }

    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps phRejectCall()
 */
static PyObject * PyPhRejectCall(PyObject *self, PyObject *params) {
    int cid;
    int reason;
    int ret = -1;

    if (PyArg_ParseTuple(params, "ii", &cid, &reason)) {
        ret = phRejectCall(cid, reason);
    }

    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps phRingingCall()
 */
static PyObject * PyPhRingingCall(PyObject *self, PyObject *params) {
    int cid;
    int ret = -1;

    if (PyArg_ParseTuple(params, "i", &cid)) {
        ret = phRingingCall(cid);
    }

    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps phCloseCall()
 */
static PyObject * PyPhCloseCall(PyObject *self, PyObject *params) {
    int cid;
    int ret = -1;

    if (PyArg_ParseTuple(params, "i", &cid)) {
        ret = phCloseCall(cid);
    }

    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps phSendMessage() => replaced by PyPhLineSendMessage
 */
/*static PyObject * PyPhSendMessage(PyObject *self, PyObject *params) {
    const char *from;
    const char *uri;
    const char *buff;
    int ret;

    if (PyArg_ParseTuple(params, "sss", &from, &uri, &buff)) {
        ret = phSendMessage(from, uri, buff);
    }

    return Py_BuildValue("i", ret);
}*/

/**
 * @brief Wraps phSubscribe() => replaced by PyPhLineSubscribe
 */
/*static PyObject * PyPhSubscribe(PyObject *self, PyObject *params) {
    const char *from;
    const char *to;
    const int winfo;
    int ret;

    if (PyArg_ParseTuple(params, "ssi", &from, &to, &winfo)) {
        ret = phSubscribe(from, to, winfo);
    }

    return Py_BuildValue("i", ret);
}*/

/**
 * @brief Wraps phPublish() => replaced by PyPhLinePublish
 */
/*static PyObject * PyPhPublish(PyObject *self, PyObject *params) {
    const char *from;
    const char *to;
    const char *content_type;
    const char *content;
    const int winfo;
    int ret;

    if (PyArg_ParseTuple(params, "ssiss", &from, &to, &winfo,
                    &content_type, &content)) {
        ret = phPublish(from, to, winfo, content_type, content);
    }

    return Py_BuildValue("i", ret);
}*/

/**
 * @brief Wraps phDelVline
 */
static PyObject * PyPhDelVline(PyObject *self, PyObject *params) {
    int vlid;
    int regTimeout;
    int ret = -1;

    if (PyArg_ParseTuple(params, "ii", &vlid, &regTimeout)) {
        ret = phDelVline(vlid, regTimeout);
    }

    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps phTransferCall
 */
static PyObject * PyPhTransferCall(PyObject *self, PyObject *params) {
    int cid;
    int targetCid;
    int ret = -1;

    if (PyArg_ParseTuple(params, "ii", &cid, &targetCid)) {
        ret = phTransferCall(cid, targetCid);
    }

    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps phBlindTransferCall
 */
static PyObject * PyPhBlindTransferCall(PyObject *self, PyObject *params) {
    int cid;
    char * uri;
    int ret = -1;

    if (PyArg_ParseTuple(params, "is", &cid, &uri)) {
        ret = phBlindTransferCall(cid, uri);
    }

    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps phResumeCall
 */
static PyObject * PyPhResumeCall(PyObject *self, PyObject *params) {
    int cid;
    int ret = -1;

    if (PyArg_ParseTuple(params, "i", &cid)) {
        ret = phResumeCall(cid);
    }

    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps phHoldCall
 */
static PyObject * PyPhHoldCall(PyObject *self, PyObject *params) {
    int cid;
    int ret = -1;

    if (PyArg_ParseTuple(params, "i", &cid)) {
        ret = phHoldCall(cid);
    }

    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps phSendDtmf
 */
static PyObject * PyPhSendDtmf(PyObject *self, PyObject *params) {
    int cid;
    int dtmfChar;
    int mode;
    int ret = -1;

    if (PyArg_ParseTuple(params, "iii", &cid, &dtmfChar, &mode)) {
        ret = phSendDtmf(cid, dtmfChar, mode);
    }

    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps phChangeAudioDevices
 */
static PyObject * PyPhChangeAudioDevices(PyObject *self, PyObject *params) {
    char * devstr;
    int ret = -1;

    if (PyArg_ParseTuple(params, "s", &devstr)) {
        ret = phChangeAudioDevices(devstr);
    }

    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps phCrash
 */
static PyObject * PyPhCrash(PyObject *self, PyObject *params) {
    int ret;
    ret = phCrash();
    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps phSetDebugLevel
 */
static PyObject * PyPhSetDebugLevel(PyObject *self, PyObject *params) {
    int level;

    if (PyArg_ParseTuple(params, "i", &level)) {
        phSetDebugLevel(level);
    }
    return Py_None;
}

/**
 * @brief Wraps phGetNatInfo
 */
static PyObject * PyPhGetNatInfo(PyObject *self, PyObject *params) {
    char * arg;
    char nattype[8];
    char firewallip[16];
    int ret;

    ret = phGetNatInfo(nattype, 8, firewallip, 16);
    if( !ret ) {

        if (PyArg_ParseTuple(params, "s", &arg)) {
            if(strcmp(arg, "type" ) == 0) {
                return Py_BuildValue("s", nattype);
            }
            if(strcmp(arg, "firewall" ) == 0) {
                return Py_BuildValue("s", firewallip);
            }
        }
    }
    return Py_None;
}


/**
 * @brief Wraps phConf
 */
static PyObject * PyPhConf(PyObject *self, PyObject *params) {
    int cid1, cid2;
    int ret = -1;

    if (PyArg_ParseTuple(params, "ii", &cid1, &cid2)) {
        printf("PyPhConf: %i / %i\n", cid1, cid2);
        ret = phConf(cid1, cid2);
    }
    return Py_BuildValue("i", ret);
}


/**
 * This function initialize the python callbacks functions that will
 * be called when a corresponding C callback from the phCallbacks
 * structure is called from the phapi stack
 *
 * @param  args The callbacks as a PyObject tuple, with the following order :
 */
static PyObject * PyPhSetCallbacks(PyObject *self, PyObject *args) {
    PyObject *result = NULL;
    PyObject *callback_call_progress;
    PyObject *callback_reg_progress;
    PyObject *callback_error_notify;
    PyObject *callback_transfer_progress;
    PyObject *callback_conf_progress;
    PyObject *callback_msg_progress;
    PyObject *callback_on_notify;
    PyObject *callback_subscription_progress;
    //PyObject *callback_frame_display;

    if (PyArg_ParseTuple(args, "OOOOOOOO",
                &callback_call_progress,
                &callback_reg_progress,
                &callback_error_notify,
                &callback_transfer_progress,
                &callback_conf_progress,
                &callback_msg_progress,
                &callback_on_notify,
                &callback_subscription_progress
                /*&callback_frame_display*/)) {

        if (!PyCallable_Check(callback_reg_progress)) {
            PyErr_SetString(PyExc_TypeError, "parameter must be callable");
            return NULL;
        }

        Py_XINCREF(callback_reg_progress);
        Py_XDECREF(pyphapi_registerProgress);

        Py_XINCREF(callback_call_progress);
        Py_XDECREF(pyphapi_callProgress);

        Py_XINCREF(callback_error_notify);
        Py_XDECREF(pyphapi_errorNotify);

        Py_XINCREF(callback_transfer_progress);
        Py_XDECREF(pyphapi_transferProgress);

        Py_XINCREF(callback_conf_progress);
        Py_XDECREF(pyphapi_confProgress);

        Py_XINCREF(callback_msg_progress);
        Py_XDECREF(pyphapi_msgProgress);

        Py_XINCREF(callback_on_notify);
        Py_XDECREF(pyphapi_onNotify);

        Py_XINCREF(callback_subscription_progress);
        Py_XDECREF(pyphapi_subscriptionProgress);

        //Py_XINCREF(callback_frame_display);
        //Py_XDECREF(pyphapi_frameDisplay);

        Py_INCREF(Py_None);
        pyphapi_registerProgress = callback_reg_progress;
        pyphapi_callProgress = callback_call_progress;
        pyphapi_errorNotify = callback_error_notify;
        pyphapi_transferProgress = callback_transfer_progress;
        pyphapi_confProgress = callback_conf_progress;
        pyphapi_msgProgress = callback_msg_progress;
        pyphapi_onNotify = callback_on_notify;
        pyphapi_subscriptionProgress = callback_subscription_progress;
        //pyphapi_frameDisplay = callback_frame_display;
        result = Py_None;
    }

    return result;
}

/**
 * @brief Lock the Python GIL (Global Interpreter Lock) and call a function
 *
 * This function first locks the GIL, and then calls a python callback
 * with some arguments. Once called it decrements ref count of the
 * arguments variable, and then releases the GIL.
 *
 * @param   callback    The function to call
 * @param   args        The arguments to pass to the function
 *
 */
void pyphapi_lock_and_call(PyObject *callback, PyObject *args) {
    PyGILState_STATE gstate;

    if (callback) {
        gstate = PyGILState_Ensure();
        PyEval_CallObject(callback, args);

        Py_DECREF(args);
        PyGILState_Release(gstate);
    }
}

/**
 * @brief callProgress() phapi's C callback
 */
void pyphapi_callback_callProgress(int cid, const phCallStateInfo_t *info) {
    PyObject *call_info;

    call_info = Py_BuildValue("(ii)", cid, info->event);
    pyphapi_lock_and_call(pyphapi_callProgress, call_info);
}

/**
 * @brief transferProgress() phapi's C callback
 */
void pyphapi_callback_transferProgress(int cid,
                const phTransferStateInfo_t *info) {
    PyObject *transfer_info;

    transfer_info = Py_BuildValue("(ii)", cid, 0);
    pyphapi_lock_and_call(pyphapi_transferProgress, transfer_info);
}

/**
 * @brief confProgress() phapi's C callback
 */
void pyphapi_callback_confProgress(int cfid,
                const phConfStateInfo_t *info) {
    PyObject *conf_info;

    conf_info = Py_BuildValue("(iiii)", cfid, info->confEvent, info->memberCid, info->errorCode);
    pyphapi_lock_and_call(pyphapi_confProgress, conf_info);
}

/**
 * @brief msgProgress() phapi's C callback
 */
void pyphapi_callback_msgProgress(int mid,const phMsgStateInfo_t *info) {
    PyObject *msg_info;

    msg_info = Py_BuildValue("(iiisssss)",
                    mid,
                    info->event,
                    info->status,
                    info->from,
                    info->to,
                    info->ctype,
                    info->subtype,
                    info->content);

    pyphapi_lock_and_call(pyphapi_msgProgress, msg_info);
}

/**
 * @brief onNotify() phapi's C callback
 */
void pyphapi_callback_onNotify(const char* event, const char* from,
                const char* content) {
    PyObject *notify_info;

    notify_info = Py_BuildValue("(sss)", event, from, content);
    pyphapi_lock_and_call(pyphapi_onNotify, notify_info);
}

/**
 * @brief subscriptionProgress() phapi's C callback
 */
void pyphapi_callback_subscriptionProgress(int sid,
                const phSubscriptionStateInfo_t *info) {
    PyObject *sub_info;

    if (pyphapi_subscriptionProgress)
    {
        sub_info = Py_BuildValue("(iiiss)", sid, info->event, info->status, info->from, info->to);
        pyphapi_lock_and_call(pyphapi_subscriptionProgress, sub_info);
    }
}

/**
 * @brief errorNotify() phapi's C callback
 */
void pyphapi_callback_errorNotify(enum phErrors error) {
    PyObject *err_info;

    if (pyphapi_errorNotify)
    {
        err_info = Py_BuildValue("(i)", (int) error);
        pyphapi_lock_and_call(pyphapi_errorNotify, err_info);
    }
}

/**
 * @brief registerProgress() phapi's C callback
 */
static void pyphapi_callback_registerProgress(int cid, int regStatus) {
    PyObject *reg_info;

    if (pyphapi_registerProgress)
    {
      reg_info = Py_BuildValue("(ii)", cid, regStatus);
      pyphapi_lock_and_call(pyphapi_registerProgress, reg_info);
    }
}

/**
 * @brief frameDisplay() phapi's C callback
 */
static void pyphapi_callback_frameDisplay(int cid, phVideoFrameReceivedEvent_t *ev) {
  PyObject *frameReceivedEvent;

  if (pyphapi_frameDisplay)
  {
    frameReceivedEvent = Py_BuildValue("(i)", &cid);
    pyphapi_lock_and_call(pyphapi_frameDisplay, frameReceivedEvent);
  }
}

/*****************************************************************************
 *                                   NEW API                                 *
 *****************************************************************************/

static PyObject *pyowpl_callProgress = NULL;
static PyObject *pyowpl_registerProgress =  NULL;
static PyObject *pyowpl_messageProgress =  NULL;
static PyObject *pyowpl_subscriptionProgress =  NULL;
static PyObject *pyowpl_onNotify = NULL;
static PyObject *pyowpl_errorNotify = NULL;

static int phApiEventsHandler(OWPL_EVENT_CATEGORY category, void* pInfo, void* pUserData);

static void pyowpl_callback_callProgress(OWPL_CALLSTATE_INFO * info);
static void pyowpl_callback_registerProgress(OWPL_LINESTATE_INFO * info);
static void pyowpl_callback_messageProgress(OWPL_MESSAGE_INFO * info);
static void pyowpl_callback_subscriptionProgress(OWPL_SUBSTATUS_INFO * info);
static void pyowpl_callback_onNotify(OWPL_NOTIFICATION_INFO * info);
static void pyowpl_callback_errorNotify(OWPL_ERROR_INFO * info);


static int phApiEventsHandler(OWPL_EVENT_CATEGORY category, void* pInfo, void* pUserData) {
    switch(category) {
        case EVENT_CATEGORY_CALLSTATE :
            pyowpl_callback_callProgress((OWPL_CALLSTATE_INFO *)pInfo);
            break;

        case EVENT_CATEGORY_LINESTATE :
            pyowpl_callback_registerProgress((OWPL_LINESTATE_INFO *)pInfo);
            break;

        case EVENT_CATEGORY_MESSAGE :
            pyowpl_callback_messageProgress((OWPL_MESSAGE_INFO *)pInfo);
            break;

        case EVENT_CATEGORY_SUB_STATUS :
            pyowpl_callback_subscriptionProgress((OWPL_SUBSTATUS_INFO *)pInfo);
            break;

        case EVENT_CATEGORY_NOTIFY :
            pyowpl_callback_onNotify((OWPL_NOTIFICATION_INFO *)pInfo);
            break;

        case EVENT_CATEGORY_ERROR :
            pyowpl_callback_errorNotify((OWPL_ERROR_INFO *)pInfo);
            break;

        default :
            break;
    }
    return 0;
}

static void pyowpl_callback_callProgress(OWPL_CALLSTATE_INFO * info) {
    PyObject * cInfo;

    if(info && pyowpl_callProgress) {
        cInfo = Py_BuildValue("(iiiis)",
            info->event,
            info->cause,
            info->hLine,
            info->hCall,
            info->szRemoteIdentity);
        pyphapi_lock_and_call(pyowpl_callProgress, cInfo);
    }
}

static void pyowpl_callback_registerProgress(OWPL_LINESTATE_INFO * info) {
    PyObject *reg_info;

    if(info && pyowpl_registerProgress) {
        reg_info = Py_BuildValue("(iii)",
            info->event,
            info->cause,
            info->hLine);
        pyphapi_lock_and_call(pyowpl_registerProgress, reg_info);
    }
}

static void pyowpl_callback_messageProgress(OWPL_MESSAGE_INFO * info) {
    PyObject * mInfo;

    if(info && pyowpl_messageProgress) {
        mInfo = Py_BuildValue("(iiisssss)",
            info->event,
            info->cause,
            info->messageId,
            info->szContentType,
            info->szSubContentType,
            info->szLocalIdentity,
            info->szRemoteIdentity,
            info->szContent);

        pyphapi_lock_and_call(pyowpl_messageProgress, mInfo);
    }
}

static void pyowpl_callback_subscriptionProgress(OWPL_SUBSTATUS_INFO * info) {
    PyObject *sInfo;

    if(info) {
        sInfo = Py_BuildValue("(iiis)",
            info->state,
            info->cause,
            info->hSub,
            info->szRemoteIdentity);
        pyphapi_lock_and_call(pyphapi_subscriptionProgress, sInfo);
    }
}

static void pyowpl_callback_onNotify(OWPL_NOTIFICATION_INFO * info) {
    PyObject * nInfo;
    OWPL_NOTIFICATION_STATUS_INFO *stInfos;


    if (info->event == NOTIFICATION_PRESENCE) {
        stInfos = (OWPL_NOTIFICATION_STATUS_INFO *) info->Data.StatusInfo;
        //FIXME: parsing the content make Python crash
        //printf("content:%s\n", info->szXmlContent);
        nInfo = Py_BuildValue("(ds)",
                              info->event,
                              stInfos->szRemoteIdentity,
                              info->szXmlContent);
        pyphapi_lock_and_call(pyowpl_onNotify, nInfo);
    }
}

static void pyowpl_callback_errorNotify(OWPL_ERROR_INFO * info) {
    PyObject * eInfo;

    if(info) {
        eInfo = Py_BuildValue("(i)",
            info->event);
        pyphapi_lock_and_call(pyowpl_errorNotify, eInfo);
    }
}

static PyObject * PyOwplEventListenerAdd(PyObject * self, PyObject * params) {
    PyObject *result = NULL;
    PyObject * callback_callProgress;
    PyObject * callback_registerProgress;
    PyObject * callback_messageProgress;
    PyObject * callback_subscriptionProgress;
    PyObject * callback_onNotify;
    PyObject * callback_errorNotify;

    int pycode;

    pycode = PyArg_ParseTuple(params, "OOOOOO",
        &callback_callProgress,
        &callback_registerProgress,
        &callback_messageProgress,
        &callback_subscriptionProgress,
        &callback_onNotify,
        &callback_errorNotify);

    if(!pycode) {
        return NULL;
    }

    if (!PyCallable_Check(callback_callProgress)) {
        PyErr_SetString(PyExc_TypeError, "parameter must be callable");
        return NULL;
    }

    if (!PyCallable_Check(callback_registerProgress)) {
        PyErr_SetString(PyExc_TypeError, "parameter must be callable");
        return NULL;
    }

    if (!PyCallable_Check(callback_messageProgress)) {
        PyErr_SetString(PyExc_TypeError, "parameter must be callable");
        return NULL;
    }

    if (!PyCallable_Check(callback_subscriptionProgress)) {
        PyErr_SetString(PyExc_TypeError, "parameter must be callable");
        return NULL;
    }

    if (!PyCallable_Check(callback_onNotify)) {
        PyErr_SetString(PyExc_TypeError, "parameter must be callable");
        return NULL;
    }

    if (!PyCallable_Check(callback_errorNotify)) {
        PyErr_SetString(PyExc_TypeError, "parameter must be callable");
        return NULL;
    }

    Py_XINCREF(callback_callProgress);
    Py_XDECREF(pyowpl_callProgress);

    Py_XINCREF(callback_registerProgress);
    Py_XDECREF(pyowpl_registerProgress);

    Py_XINCREF(callback_messageProgress);
    Py_XDECREF(pyowpl_messageProgress);

    Py_XINCREF(callback_subscriptionProgress);
    Py_XDECREF(pyowpl_subscriptionProgress);

    Py_XINCREF(callback_onNotify);
    Py_XDECREF(pyowpl_onNotify);

    Py_XINCREF(callback_errorNotify);
    Py_XDECREF(pyowpl_errorNotify);

    Py_INCREF(Py_None);
    result = Py_None;

    pyowpl_callProgress = callback_callProgress;
    pyowpl_registerProgress = callback_registerProgress;
    pyowpl_messageProgress = callback_messageProgress;
    pyowpl_subscriptionProgress = callback_subscriptionProgress;
    pyowpl_onNotify = callback_onNotify;
    pyowpl_errorNotify = callback_errorNotify;

    owplEventListenerAdd(phApiEventsHandler, NULL);

    return result;
}

/**
 * @brief Wraps owplInit()
 */
static PyObject * PyOwplInit(PyObject * self, PyObject * params) {
    const int asyncCallbackMode;
    const int udpPort;
    const int tcpPort;
    const int tlsPort;
    const char* szBindToAddr;
    const int bUserSequentialPorts;

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "iiiisi",
        &asyncCallbackMode,
        &udpPort,
        &tcpPort,
        &tlsPort,
        &szBindToAddr,
        &bUserSequentialPorts);

    if(!pycode) {
        return Py_None;
    }

    ret = owplInit(asyncCallbackMode,
        udpPort,
        tcpPort,
        tlsPort,
        szBindToAddr,
        bUserSequentialPorts);
    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps owplShutdown()
 */
static PyObject * PyOwplShutdown(PyObject * self, PyObject * params) {
    int ret;
    ret = owplShutdown();
    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps owplConfigSetLocalHttpProxy()
 */
static PyObject * PyOwplConfigSetLocalHttpProxy(PyObject *self, PyObject *params) {
    const char* szLocalProxyAddr;
    const int LocalProxyPort;
    const char* szLocalProxyUserName;
    const char* szLocalProxyPasswd;

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "siss",
        &szLocalProxyAddr,
        &LocalProxyPort,
        &szLocalProxyUserName,
        &szLocalProxyPasswd);

    if (!pycode) {
        return Py_None;
    }

    ret = owplConfigSetLocalHttpProxy(szLocalProxyAddr,
        LocalProxyPort,
        szLocalProxyUserName,
        szLocalProxyPasswd);
    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps owplConfigSetTunnel
 */
static PyObject * PyOwplConfigSetTunnel(PyObject *self, PyObject *params) {
    const char* szTunnelSeverAddr;
    const int TunnelServerPort;
    const int TunnelMode;

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "sii",
        &szTunnelSeverAddr,
        &TunnelServerPort,
        &TunnelMode);

    if (!pycode) {
        return Py_None;
    }

    ret = owplConfigSetTunnel(szTunnelSeverAddr,
        TunnelServerPort,
        TunnelMode);
    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps owplConfigSetNat
 */
static PyObject * PyOwplConfigSetNat(PyObject *self, PyObject *params) {
    const OWPL_NAT_TYPE eNatType;
    const int natRefreshTime;

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "ii",
        &eNatType,
        &natRefreshTime);

    if (!pycode) {
        return Py_None;
    }

    ret = owplConfigSetNat(eNatType,
            natRefreshTime);
    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps owplConfigSetOutboundProxy
 */
static PyObject * PyOwplConfigSetOutboundProxy(PyObject *self, PyObject *params) {
    const char * szProxyAddr;

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "s",
        &szProxyAddr);

    if (!pycode) {
        return Py_None;
    }

    ret = owplConfigSetOutboundProxy(szProxyAddr);
    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps owplConfigAddAudioCodecByName
 */
static PyObject * PyOwplConfigAddAudioCodecByName(PyObject *self, PyObject *params) {
    const char* szCodecName;

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "s",
        &szCodecName);

    if (!pycode) {
        return Py_None;
    }

    ret = owplConfigAddAudioCodecByName(szCodecName);
    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps owplConfigSetAudioCodecs
 */
static PyObject * PyOwplConfigSetAudioCodecs(PyObject *self, PyObject *params) {
    const char* szCodecList;

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "s",
        &szCodecList);

    if (!pycode) {
        return Py_None;
    }

    ret = owplConfigSetAudioCodecs(szCodecList);
    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps owplConfigAddVideoCodecByName
 */
static PyObject * PyOwplConfigAddVideoCodecByName(PyObject *self, PyObject *params) {
    const char* szCodecName;

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "s",
        &szCodecName);

    if (!pycode) {
        return Py_None;
    }

    ret = owplConfigAddVideoCodecByName(szCodecName);
    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps owplConfigSetVideoCodecs
 */
static PyObject * PyOwplConfigSetVideoCodecs(PyObject *self, PyObject *params) {
    const char* szCodecList;

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "s",
        &szCodecList);

    if (!pycode) {
        return Py_None;
    }

    ret = owplConfigSetVideoCodecs(szCodecList);
    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps owplConfigGetBoundLocalAddr
 */
static PyObject * PyOwplConfigGetBoundLocalAddr(PyObject *self, PyObject *params) {
    char szLocalAddr[MAX_URI_LENGTH]; // not to be passed by Python

    int ret;

    ret = owplConfigGetBoundLocalAddr(szLocalAddr, sizeof(szLocalAddr));

    if(ret == OWPL_RESULT_SUCCESS) {
        return Py_BuildValue("s", szLocalAddr);
    }

    return Py_None;
}

/**
 * @brief Wraps owplConfigLocalHttpProxyGetAddr
 */
static PyObject * PyOwplConfigLocalHttpProxyGetAddr(PyObject *self, PyObject *params) {
    char szLocalProxyAddr[MAX_IP_LENGTH]; // not to be passed by Python

    int ret;

    ret = owplConfigGetBoundLocalAddr(szLocalProxyAddr,
        sizeof(szLocalProxyAddr));

    if(ret == OWPL_RESULT_SUCCESS) {
        return Py_BuildValue("s", szLocalProxyAddr);
    }

    return Py_None;
}

/**
 * @brief Wraps owplConfigLocalHttpProxyGetPasswd
 */
static PyObject * PyOwplConfigLocalHttpProxyGetPasswd(PyObject *self, PyObject *params) {
    char szLocalProxyPasswd[MAX_PASSWORD_LENGTH]; // not to be passed by Python

    int ret;

    ret = owplConfigGetBoundLocalAddr(szLocalProxyPasswd,
        sizeof(szLocalProxyPasswd));

    if(ret == OWPL_RESULT_SUCCESS) {
        return Py_BuildValue("s", szLocalProxyPasswd);
    }

    return Py_None;
}

/**
 * @brief Wraps owplConfigLocalHttpProxyGetPort
 */
static PyObject * PyOwplConfigLocalHttpProxyGetPort(PyObject *self, PyObject *params) {
    int LocalProxyPort; // not to be passed by Python

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "i",
        &LocalProxyPort);

    if (!pycode) {
        return Py_None;
    }

    ret = owplConfigLocalHttpProxyGetPort(&LocalProxyPort);

    if(ret == OWPL_RESULT_SUCCESS) {
        return Py_BuildValue("i", LocalProxyPort);
    }

    return Py_None;
}

/**
 * @brief Wraps owplConfigLocalHttpProxyGetUserName
 */
static PyObject * PyOwplConfigLocalHttpProxyGetUserName(PyObject *self, PyObject *params) {
    char szLocalProxyUserName[MAX_USERNAME_LENGTH]; // not to be passed by Python

    int ret;

    ret = owplConfigLocalHttpProxyGetUserName(szLocalProxyUserName,
        sizeof(szLocalProxyUserName));

    if(ret == OWPL_RESULT_SUCCESS) {
        return Py_BuildValue("s", szLocalProxyUserName);
    }

    return Py_None;
}

/**
 * @brief Wraps owplAudioSetConfigString
 */
static PyObject * PyOwplAudioSetConfigString(PyObject *self, PyObject *params) {
    const char* szAudioConfig;

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "s",
        &szAudioConfig);

    if (!pycode) {
        return Py_None;
    }

    ret = owplAudioSetConfigString(szAudioConfig);
    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps owplLineAdd
 */
static PyObject * PyOwplLineAdd(PyObject *self, PyObject *params) {
    const char* displayname;
    const char* username;
    const char *server;
    const char*  proxy;
    int regTimeout;
    OWPL_LINE hLine; // not to be passed by Python

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "ssssi",
        &displayname,
        &username,
        &server,
        &proxy,
        &regTimeout);

    if (!pycode) {
        return Py_None;
    }

    ret = owplLineAdd(displayname,
        username,
        server,
        proxy,
        regTimeout,
        &hLine);

    if(ret == OWPL_RESULT_SUCCESS) {
        return Py_BuildValue("i", hLine);
    }

    return Py_None;
}

/**
 * @brief Wraps owplLineDelete
 */
static PyObject * PyOwplLineDelete(PyObject *self, PyObject *params) {
    const OWPL_LINE hLine;
    const unsigned short skipUnregister;

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "ii",
        &hLine,
        &skipUnregister);

    if (!pycode) {
        return Py_None;
    }

    ret = owplLineDelete(hLine, skipUnregister);
    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps owplLineGetProxy
 */
static PyObject * PyOwplLineGetProxy(PyObject *self, PyObject *params) {
    const OWPL_LINE hLine;
    char szBuffer[MAX_IP_LENGTH]; // not to be passed by Python
    int nBuffer; // not to be passed by Python

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "i",
        &hLine);

    if (!pycode) {
        return Py_None;
    }

    ret = owplLineGetProxy(hLine,
        szBuffer,
        &nBuffer);

    if(ret == OWPL_RESULT_SUCCESS) {
        return Py_BuildValue("s", szBuffer);
    }

    return Py_None;
}

/**
 * @brief Wraps owplLineGetLocalUserName
 */
static PyObject * PyOwplLineGetLocalUserName(PyObject *self, PyObject *params) {
    const OWPL_LINE hLine;
    char szLocalUserName[MAX_USERNAME_LENGTH]; // not to be passed by Python
    int nBuffer = sizeof(szLocalUserName); // not to be passed by Python

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "i",
        &hLine);

    if (!pycode) {
        return Py_None;
    }

    ret = owplLineGetLocalUserName(hLine,
        szLocalUserName,
        &nBuffer);

    if(ret == OWPL_RESULT_SUCCESS) {
        return Py_BuildValue("s", szLocalUserName);
    }
    return Py_None;
}

/**
 * @brief Wraps owplLineRegister
 */
static PyObject * PyOwplLineRegister(PyObject *self, PyObject *params) {
    const OWPL_LINE hLine;
    const int bRegister;

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "ii",
        &hLine,
        &bRegister);

    if (!pycode) {
        return Py_None;
    }

    ret = owplLineRegister(hLine,
        bRegister);
    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps owplLineSetOpts
 */
static PyObject * PyOwplLineSetOpts(PyObject *self, PyObject *params) {
    const OWPL_LINE hLine;
    LineOptEnum Opt;
    const void *Data;

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "iiO",
        &hLine,
        &Opt,
        &Data);

    if (!pycode) {
        return Py_None;
    }

    ret = owplLineSetOpts(hLine,
        Opt,
        Data);
    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps owplLineGetOpts (not yet implemented)
 */
/*
static PyObject * PyOwplLineGetOpts(PyObject *self, PyObject *params) {
    const OWPL_LINE hLine;
    LineOptEnum Opt;
    const void *OutBuff = NULL; // not to be passed by Python
    const int BuffSize;

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "iii",
        &hLine,
        &Opt,
        &BuffSize);

    if (!pycode) {
        return Py_None;
    }

    ret = owplLineGetOpts(hLine,
        Opt,
        OutBuff,
        BuffSize);

    if(ret == OWPL_RESULT_SUCCESS) {
        return Py_BuildValue("O", OutBuff);
    }
    return Py_None;
}
*/

/**
 * @brief Wraps owplLineGetUri
 */
static PyObject * PyOwplLineGetUri(PyObject *self, PyObject *params) {
    const OWPL_LINE hLine;
    char szBuffer[MAX_URI_LENGTH]; // not to be passed by Python
    int nBuffer = sizeof(szBuffer); // not to be passed by Python

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "i",
        &hLine);

    if (!pycode) {
        return Py_None;
    }

    ret = owplLineGetUri(hLine,
        szBuffer,
        &nBuffer);

    if(ret == OWPL_RESULT_SUCCESS) {
        return Py_BuildValue("s", szBuffer);
    }
    return Py_None;
}

/**
 * @brief Wraps owplLineAddCredential()
 */
static PyObject * PyOwplLineAddCredential(PyObject *self, PyObject *params) {
    const OWPL_LINE hLine;
    const char* szUserID;
    const char* szPasswd;
    const char* szRealm;

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "isss",
        &hLine,
        &szUserID,
        &szPasswd,
        &szRealm);

    if (!pycode) {
        return Py_None;
    }

    ret = owplLineAddCredential(hLine,
        szUserID,
        szPasswd,
        szRealm);
    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps owplLineSetBusy()
 */
static PyObject * PyOwplLineSetBusy(PyObject *self, PyObject *params) {
    const OWPL_LINE hLine;
    const unsigned short bBusy;

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "ii",
        &hLine,
        &bBusy);

    if (!pycode) {
        return Py_None;
    }

    ret = owplLineSetBusy(hLine,
        bBusy);
    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps owplLineIsBusy()
 */
static PyObject * PyOwplLineIsBusy(PyObject *self, PyObject *params) {
    const OWPL_LINE hLine;
    unsigned int bBusy;

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "i",
        &hLine);

    ret = owplLineIsBusy(hLine, &bBusy);

    if(ret == OWPL_RESULT_SUCCESS) {
        return Py_BuildValue("i", bBusy);
    }
    return Py_None;
}

/**
 * @brief Wraps owplCallCreate
 */
static PyObject * PyOwplCallCreate(PyObject *self, PyObject *params) {
    const OWPL_LINE hLine;
    OWPL_CALL hCall; // not to be passed by Python

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "i",
        &hLine);

    if (!pycode) {
        return Py_None;
    }

    ret = owplCallCreate(hLine,
        &hCall);

    if(ret == OWPL_RESULT_SUCCESS) {
        return Py_BuildValue("i", hCall);
    }
    return Py_None;
}

/**
 * @brief Wraps owplCallConnect
 */
static PyObject * PyOwplCallConnect(PyObject *self, PyObject *params) {
    const OWPL_CALL hCall;
    const char* szAddress;
    int mediaStreams;

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "isi",
        &hCall,
        &szAddress,
        &mediaStreams);

    if (!pycode) {
        return Py_None;
    }

    ret = owplCallConnect(hCall,
        szAddress,
        mediaStreams);
    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps owplCallConnectWithBody
 */
static PyObject * PyOwplCallConnectWithBody(PyObject *self, PyObject *params) {
    const OWPL_CALL hCall;
    const char* szAddress;
    const char* szContentType;
    const char* szBody;
    int BodySize;

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "isssi",
        &hCall,
        &szAddress,
        &szContentType,
        &szBody,
        &BodySize);

    if (!pycode) {
        return Py_None;
    }

    ret = owplCallConnectWithBody(hCall,
        szAddress,
        szContentType,
        szBody,
        BodySize);
    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps owplCallAccept
 */
static PyObject * PyOwplCallAccept(PyObject *self, PyObject *params) {
    const OWPL_CALL hCall;
    int mediaStreams;

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "ii",
        &hCall,
        &mediaStreams);

    if (!pycode) {
        return Py_None;
    }

    ret = owplCallAccept(hCall,
        mediaStreams);
    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps owplCallAnswer
 */
static PyObject * PyOwplCallAnswer(PyObject *self, PyObject *params) {
    const OWPL_CALL hCall;
    int mediaStreams;

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "ii",
        &hCall,
        &mediaStreams);

    if (!pycode) {
        return Py_None;
    }

    ret = owplCallAnswer(hCall,
        mediaStreams);
    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps owplCallAnswerWithBody
 */
static PyObject * PyOwplCallAnswerWithBody(PyObject *self, PyObject *params) {
    const OWPL_CALL hCall;
    const char* szContentType;
    const char* szBody;
    int BodySize;

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "issi",
        &hCall,
        &szContentType,
        &szBody,
        &BodySize);

    if (!pycode) {
        return Py_None;
    }

    ret = owplCallAnswerWithBody(hCall,
        szContentType,
        szBody,
        BodySize);
    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps owplCallReject
 */
static PyObject * PyOwplCallReject(PyObject *self, PyObject *params) {
    const OWPL_CALL hCall;
    const int errorCode;
    const char* szErrorText;

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "iis",
        &hCall,
        &errorCode,
        &szErrorText);

    if (!pycode) {
        return Py_None;
    }

    ret = owplCallReject(hCall,
        errorCode,
        szErrorText);
    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps owplCallRejectWithPredefinedReason
 */
static PyObject * PyOwplCallRejectWithPredefinedReason(PyObject *self, PyObject *params) {
    const OWPL_CALL hCall;
    OWPL_CALL_REFUSED_REASON Reason;

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "ii",
        &hCall,
        &Reason);

    if (!pycode) {
        return Py_None;
    }

    ret = owplCallRejectWithPredefinedReason(hCall,
        Reason);
    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps owplCallHold
 */
static PyObject * PyOwplCallHold(PyObject *self, PyObject *params) {
    const OWPL_CALL hCall;

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "i",
        &hCall);

    if (!pycode) {
        return Py_None;
    }

    ret = owplCallHold(hCall);
    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps owplCallHoldWithBody
 */
static PyObject * PyOwplCallHoldWithBody(PyObject *self, PyObject *params) {
    const OWPL_CALL hCall;
    const char* szContentType;
    const char* szBody;
    int BodySize;

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "issi",
        &hCall,
        &szContentType,
        &szBody,
        &BodySize);

    if (!pycode) {
        return Py_None;
    }

    ret = owplCallHoldWithBody(hCall,
        szContentType,
        szBody,
        BodySize);
    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps owplCallUnhold
 */
static PyObject * PyOwplCallUnhold(PyObject *self, PyObject *params) {
    const OWPL_CALL hCall;

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "i",
        &hCall);

    if (!pycode) {
        return Py_None;
    }

    ret = owplCallUnhold(hCall);
    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps owplCallUnholdWithBody
 */
static PyObject * PyOwplCallUnholdWithBody(PyObject *self, PyObject *params) {
    const OWPL_CALL hCall;
    const char* szContentType;
    const char* szBody;
    int BodySize;

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "issi",
        &hCall,
        &szContentType,
        &szBody,
        &BodySize);

    if (!pycode) {
        return Py_None;
    }

    ret = owplCallUnholdWithBody(hCall,
        szContentType,
        szBody,
        BodySize);
    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps owplCallDisconnect
 */
static PyObject * PyOwplCallDisconnect(PyObject *self, PyObject *params) {
    const OWPL_CALL hCall;

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "i",
        &hCall);

    if (!pycode) {
        return Py_None;
    }

    ret = owplCallDisconnect(hCall);
    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps owplCallGetLocalID (not yet implemented)
 */
/*
static PyObject * PyOwplCallGetLocalID(PyObject *self, PyObject *params) {
    const OWPL_CALL hCall;
    char szId[MAX_URI_LENGTH]; // not to be passed by Python

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "i",
        &hCall);

    if (!pycode) {
        return Py_None;
    }

    ret = owplCallGetLocalID(hCall,
        szId,
        sizeof(szId));

    if(ret == OWPL_RESULT_SUCCESS) {
        return Py_BuildValue("s", szId);
    }
    return Py_None;
}
*/

/**
 * @brief Wraps owplCallGetRemoteID (not yet implemented)
 */
/*
static PyObject * PyOwplCallGetRemoteID(PyObject *self, PyObject *params) {
    const OWPL_CALL hCall;
    char szId[MAX_URI_LENGTH]; // not to be passed by Python

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "i",
        &hCall);

    if (!pycode) {
        return Py_None;
    }

    ret = owplCallGetRemoteID(hCall,
        szId,
        sizeof(szId));

    if(ret == OWPL_RESULT_SUCCESS) {
        return Py_BuildValue("s", szId);
    }
    return Py_None;
}
*/

/**
 * @brief Wraps owplCallSetAssertedId (not yet implemented)
 */
/*
static PyObject * PyOwplCallSetAssertedId(PyObject *self, PyObject *params) {
    const OWPL_CALL hCall;
    const char* szPAssertedId;
    const int bSignalNow;

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "isi",
        &hCall,
        &szPAssertedId,
        &bSignalNow);

    if (!pycode) {
        return Py_None;
    }

    ret = owplCallSetAssertedId(hCall,
        szPAssertedId,
        bSignalNow);
    return Py_BuildValue("i", ret);
}
*/

/**
 * @brief Wraps owplCallGetRemoteContact (not yet implemented)
 */
/*
static PyObject * PyOwplCallGetRemoteContact(PyObject *self, PyObject *params) {
    const OWPL_CALL hCall;
    char szContact[MAX_USERNAME_LENGTH]; // not to be passed by Python

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "i",
        &hCall);

    if (!pycode) {
        return Py_None;
    }

    ret = owplCallGetRemoteContact(hCall,
        szContact,
        sizeof(szContact));


    if(ret == OWPL_RESULT_SUCCESS) {
        return Py_BuildValue("s", szContact);
    }
    return Py_None;
}
*/

/**
 * @brief Wraps owplCallToneStart (not yet implemented)
 */
/*
static PyObject * PyOwplCallToneStart(PyObject *self, PyObject *params) {
    const OWPL_CALL hCall;
    const OWPL_TONE_ID toneId;
    const int bLocal;
    const int bRemote;

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "iiii",
        &hCall,
        &toneId,
        &bLocal,
        &bRemote);

    if (!pycode) {
        return Py_None;
    }

    ret = owplCallToneStart(hCall,
        toneId,
        bLocal,
        bRemote);
    return Py_BuildValue("i", ret);
}
*/

/**
 * @brief Wraps owplCallToneStop (not yet implemented)
 */
/*
static PyObject * PyOwplCallToneStop(PyObject *self, PyObject *params) {
    const OWPL_CALL hCall;

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "i",
        &hCall);

    if (!pycode) {
        return Py_None;
    }

    ret = owplCallToneStop(hCall);
    return Py_BuildValue("i", ret);
}
*/

/**
 * @brief Wraps owplCallTonePlay (not yet implemented)
 */
/*
static PyObject * PyOwplCallTonePlay(PyObject *self, PyObject *params) {
    const OWPL_CALL hCall;
    unsigned long Miliseconds;

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "ik",
        &hCall,
        &Miliseconds);

    if (!pycode) {
        return Py_None;
    }

    ret = owplCallTonePlay(hCall,
        Miliseconds);
    return Py_BuildValue("i", ret);
}
*/

/**
 * @brief Wraps owplCallAudioPlayFileStart (not yet implemented)
 */
/*
static PyObject * PyOwplCallAudioPlayFileStart(PyObject *self, PyObject *params) {
    const OWPL_CALL hCall;
    const char* szFile;
    const int bRepeat;
    const int bLocal;
    const int bRemote;

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "isiii",
        &hCall,
        &szFile,
        &bRepeat,
        &bLocal,
        &bRemote);

    if (!pycode) {
        return Py_None;
    }

    ret = owplCallAudioPlayFileStart(hCall,
        szFile,
        bRepeat,
        bLocal,
        bRemote);
    return Py_BuildValue("i", ret);
}
*/

/**
 * @brief Wraps owplCallAudioPlayFileStop (not yet implemented)
 */
/*
static PyObject * PyOwplCallAudioPlayFileStop(PyObject *self, PyObject *params) {
    const OWPL_CALL hCall;

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "i",
        &hCall);

    if (!pycode) {
        return Py_None;
    }

    ret = owplCallAudioPlayFileStop(hCall);
    return Py_BuildValue("i", ret);
}
*/

/**
 * @brief Wraps owplCallSendInfo  (not yet implemented)
 */
/*
static PyObject * PyOwplCallSendInfo(PyObject *self, PyObject *params) {
    const OWPL_CALL hCall;
    const char* szContentType;
    const char* szContent;
    const int nContentLength;

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "issi",
        &hCall,
        &szContentType,
        &szContent,
        &nContentLength);

    if (!pycode) {
        return Py_None;
    }

    ret = owplCallSendInfo(hCall,
        szContentType,
        szContent,
        nContentLength);
    return Py_BuildValue("i", ret);
}
*/

/**
 * @brief Wraps owplPresenceSubscribe
 */
static PyObject * PyOwplPresenceSubscribe(PyObject *self, PyObject *params) {
    OWPL_LINE  hLine;
    const char* szUri;
    const int winfo;
    OWPL_SUB hSub; // not to be passed by Python

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "isi",
        &hLine,
        &szUri,
        &winfo);

    if (!pycode) {
        return Py_None;
    }

    ret = owplPresenceSubscribe(hLine,
        szUri,
        winfo,
        &hSub);

    if(ret == OWPL_RESULT_SUCCESS) {
        return Py_BuildValue("i", hSub);
    }
    return Py_None;
}

/**
 * @brief Wraps owplPresenceUnsubscribe
 */
static PyObject * PyOwplPresenceUnsubscribe(PyObject *self, PyObject *params) {
    const char* szRemoteUri;

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "s",
        &szRemoteUri);

    if (!pycode) {
        return Py_None;
    }

    ret = owplPresenceUnsubscribe(szRemoteUri);
    return Py_BuildValue("i", ret);
}

/**
 * @brief Wraps owplPresencePublish
 */
static PyObject * PyOwplPresencePublish(PyObject *self, PyObject *params) {
    OWPL_LINE  hLine;
    const int Online;
    const char * szStatus;
    OWPL_PUB hPub; // not to be passed by Python

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "iis",
        &hLine,
        &Online,
        &szStatus);

    if (!pycode) {
        return Py_None;
    }

    ret = owplPresencePublish(hLine,
        Online,
        szStatus,
        &hPub);

    if(ret == OWPL_RESULT_SUCCESS) {
        return Py_BuildValue("i", hPub);
    }
    return Py_None;
}

/**
 * @brief Wraps owplMessageSend
 */
static PyObject * PyOwplMessageSend(PyObject *self, PyObject *params) {
    OWPL_LINE hLine;
    const char * szRemoteUri;
    const char * szContent;
    const char * szMIME;
    int messageId; // not to be passed by Python

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "isss",
        &hLine,
        &szRemoteUri,
        &szContent,
        &szMIME);

    if (!pycode) {
        return Py_None;
    }

    ret = owplMessageSend(hLine,
        szRemoteUri,
        szContent,
        szMIME,
        &messageId);

    if(ret == OWPL_RESULT_SUCCESS) {
        return Py_BuildValue("i", messageId);
    }
    return Py_None;
}

/**
 * @brief Wraps owplMessageSendPlainText
 */
static PyObject * PyOwplMessageSendPlainText(PyObject *self, PyObject *params) {
    OWPL_LINE hLine;
    const char * szRemoteUri;
    const char * szContent;
    int messageId; // not to be passed by Python

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "iss",
        &hLine,
        &szRemoteUri,
        &szContent);

    if (!pycode) {
        return Py_None;
    }

    ret = owplMessageSendPlainText(hLine,
        szRemoteUri,
        szContent,
        &messageId);

    if(ret == OWPL_RESULT_SUCCESS) {
        return Py_BuildValue("i", messageId);
    }
    return Py_None;
}

/**
 * @brief Wraps owplMessageSendTypingState
 */
static PyObject * PyOwplMessageSendTypingState(PyObject *self, PyObject *params) {
    OWPL_LINE hLine;
    const char * szRemoteUri;
    OWPL_TYPING_STATE state;
    int messageId; // not to be passed by Python

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "isi",
        &hLine,
        &szRemoteUri,
        &state);

    if (!pycode) {
        return Py_None;
    }

    ret = owplMessageSendTypingState(hLine,
        szRemoteUri,
        state,
        &messageId);

    if(ret == OWPL_RESULT_SUCCESS) {
        return Py_BuildValue("i", messageId);
    }
    return Py_None;
}

/**
 * @brief Wraps owplMessageSendIcon
 */
static PyObject * PyOwplMessageSendIcon(PyObject *self, PyObject *params) {
    OWPL_LINE hLine;
    const char * szRemoteUri;
    const char * szIconFileName;
    int messageId;

    int pycode, ret;

    pycode = PyArg_ParseTuple(params,
        "iss",
        &hLine,
        &szRemoteUri,
        &szIconFileName);

    if (!pycode) {
        return Py_None;
    }

    ret = owplMessageSendIcon(hLine,
        szRemoteUri,
        szIconFileName,
        &messageId);

    if(ret == OWPL_RESULT_SUCCESS) {
        return Py_BuildValue("i", messageId);
    }
    return Py_None;
}

/*****************************************************************************
 *                                   HELPERS                                 *
 *****************************************************************************/

/**
 * @brief wrap access to phcfg
 */
static PyObject * PyPhCfgSetS(PyObject *self, PyObject *params) {
    char * field;
    char * value_string;
    phConfig_t *cfg;

    cfg = phGetConfig();

    if (PyArg_ParseTuple(params, "ss", &field, &value_string)) {
        if( strcmp(field, "local_rtp_port" )==0) {strncpy(cfg->local_rtp_port, value_string,16);}
        if( strcmp(field, "local_audio_rtcp_port" )==0) {strncpy(cfg->local_audio_rtcp_port, value_string,16);}
        if( strcmp(field, "local_video_rtp_port" )==0) {strncpy(cfg->local_video_rtp_port, value_string,16);}
        if( strcmp(field, "local_video_rtcp_port" )==0) {strncpy(cfg->local_video_rtcp_port, value_string,16);}
        if( strcmp(field, "sipport" )==0) {strncpy(cfg->sipport, value_string,16);}
        if( strcmp(field, "nattype" )==0) {strncpy(cfg->nattype, value_string,16);}
        if( strcmp(field, "audio_codecs" )==0) {strncpy(cfg->audio_codecs, value_string,128);}
        if( strcmp(field, "video_codecs" )==0) {strncpy(cfg->video_codecs, value_string,128);}
        if( strcmp(field, "audio_dev" )==0) {strncpy(cfg->audio_dev, value_string,64);}
        if( strcmp(field, "stunserver" )==0) {strncpy(cfg->stunserver, value_string,128);}
        if( strcmp(field, "httpt_server" )==0) {strncpy(cfg->httpt_server, value_string,128);}
        if( strcmp(field, "http_proxy" )==0) {strncpy(cfg->http_proxy, value_string,128);}
        if( strcmp(field, "http_proxy_user" )==0) {strncpy(cfg->http_proxy_user, value_string,128);}
        if( strcmp(field, "http_proxy_passwd" )==0) {strncpy(cfg->http_proxy_passwd, value_string,128);}
        if( strcmp(field, "plugin_path" )==0) {strncpy(cfg->plugin_path, value_string,256);}
    }
    return Py_None;
}


/**
 * @brief wrap access to phcfg
 */
static PyObject * PyPhCfgSetI(PyObject *self, PyObject *params) {
    char * field;
    int value_int;
    phConfig_t *cfg;

    cfg = phGetConfig();

    if (PyArg_ParseTuple(params, "si", &field, &value_int)) {
        if( strcmp(field, "asyncmode" ) == 0) {cfg->asyncmode=value_int;}
        if( strcmp(field, "nomedia" ) == 0) {cfg->nomedia=value_int;}
        if( strcmp(field, "noaec" ) == 0) {cfg->noaec=value_int;}
        if( strcmp(field, "vad" ) == 0) {cfg->vad=value_int;}
        if( strcmp(field, "cng" ) == 0) {cfg->cng=value_int;}
        if( strcmp(field, "hdxmode" ) == 0) {cfg->hdxmode=value_int;}
        if( strcmp(field, "nat_refresh_time" ) == 0) {cfg->nat_refresh_time=value_int;}
        if( strcmp(field, "jitterdepth" ) == 0) {cfg->jitterdepth=value_int;}
        if( strcmp(field, "autoredir" ) == 0) {cfg->autoredir=value_int;}
        if( strcmp(field, "use_tunnel" ) == 0) {cfg->use_tunnel=value_int;}
        if( strcmp(field, "httpt_server_port" ) == 0) {cfg->httpt_server_port=value_int;}
        if( strcmp(field, "http_proxy_port" ) == 0) {cfg->http_proxy_port=value_int;}
    }
    return Py_None;
}

/*****************************************************************************
 *                         PYTHON MODULE DECLARATION                         *
 *****************************************************************************/

/*
 * Convenient define to declare a function with variable arguments in
 * the module's API table
 *
 */
#define PY_PHAPI_FUNCTION_DECL(n,f) \
    { n, (PyCFunction) f, METH_VARARGS, "" }

#define PY_PHAPI_FUNCTION_DECL_NULL \
    { NULL, NULL, 0, NULL }

/**
 * @brief Declaration of the module API table
 */
static PyMethodDef pyphapi_funcs[] = {
    /*{ "pyphapi",(PyCFunction) pyphapi, METH_NOARGS,  "Python Module of phApi"},*/
    PY_PHAPI_FUNCTION_DECL("phInit",                PyPhInit),
    PY_PHAPI_FUNCTION_DECL("phAddAuthInfo",         PyPhAddAuthInfo),
    PY_PHAPI_FUNCTION_DECL("phAddVline",            PyPhAddVline),
    PY_PHAPI_FUNCTION_DECL("phAddVline2",           PyPhAddVline2),
    PY_PHAPI_FUNCTION_DECL("phSetCallbacks",        PyPhSetCallbacks),
    PY_PHAPI_FUNCTION_DECL("phPoll",                PyPhPoll),
    PY_PHAPI_FUNCTION_DECL("phRefresh",             PyPhRefresh),
    PY_PHAPI_FUNCTION_DECL("phTerminate",           PyPhTerminate),
    PY_PHAPI_FUNCTION_DECL("phTunnelConfig",        PyPhTunnelConfig),
    PY_PHAPI_FUNCTION_DECL("phLinePlaceCall",       PyPhLinePlaceCall),
    PY_PHAPI_FUNCTION_DECL("phLinePlaceCall2",      PyPhLinePlaceCall2),
    PY_PHAPI_FUNCTION_DECL("phLineSendOptions",     PyPhLineSendOptions),
    PY_PHAPI_FUNCTION_DECL("phLineSendMessage",     PyPhLineSendMessage),
    PY_PHAPI_FUNCTION_DECL("phLineSubscribe",       PyPhLineSubscribe),
    PY_PHAPI_FUNCTION_DECL("phLineSetFollowMe",     PyPhLineSetFollowMe),
    PY_PHAPI_FUNCTION_DECL("phLineSetBusy",         PyPhLineSetBusy),
    PY_PHAPI_FUNCTION_DECL("phLinePublish",         PyPhLinePublish),
    PY_PHAPI_FUNCTION_DECL("phAcceptCall2",         PyPhAcceptCall2),
    PY_PHAPI_FUNCTION_DECL("phAcceptCall3",         PyPhAcceptCall3),
    PY_PHAPI_FUNCTION_DECL("phRejectCall",          PyPhRejectCall),
    PY_PHAPI_FUNCTION_DECL("phCloseCall",           PyPhCloseCall),
    PY_PHAPI_FUNCTION_DECL("phRingingCall",         PyPhRingingCall),
    PY_PHAPI_FUNCTION_DECL("phDelVline",            PyPhDelVline),
    PY_PHAPI_FUNCTION_DECL("phTransferCall",        PyPhTransferCall),
    PY_PHAPI_FUNCTION_DECL("phBlindTransferCall",   PyPhBlindTransferCall),
    PY_PHAPI_FUNCTION_DECL("phResumeCall",          PyPhResumeCall),
    PY_PHAPI_FUNCTION_DECL("phHoldCall",            PyPhHoldCall),
    PY_PHAPI_FUNCTION_DECL("phCfgSetS",             PyPhCfgSetS),
    PY_PHAPI_FUNCTION_DECL("phCfgSetI",             PyPhCfgSetI),
    PY_PHAPI_FUNCTION_DECL("phSendDtmf",            PyPhSendDtmf),
    PY_PHAPI_FUNCTION_DECL("phChangeAudioDevices",  PyPhChangeAudioDevices),
    PY_PHAPI_FUNCTION_DECL("phCrash",               PyPhCrash),
    PY_PHAPI_FUNCTION_DECL("phSetDebugLevel",       PyPhSetDebugLevel),
    PY_PHAPI_FUNCTION_DECL("phGetNatInfo",          PyPhGetNatInfo),
    PY_PHAPI_FUNCTION_DECL("phConf",                PyPhConf),
    /* Do not exist anymore */
    /*PY_PHAPI_FUNCTION_DECL("phSendMessage",         PyPhSendMessage),
    PY_PHAPI_FUNCTION_DECL("phSubscribe",           PyPhSubscribe),
    PY_PHAPI_FUNCTION_DECL("phPublish",             PyPhPublish),*/

    PY_PHAPI_FUNCTION_DECL("owplInit",                              PyOwplInit),
    PY_PHAPI_FUNCTION_DECL("owplShutdown",                          PyOwplShutdown),
    PY_PHAPI_FUNCTION_DECL("owplEventListenerAdd",                  PyOwplEventListenerAdd),
    PY_PHAPI_FUNCTION_DECL("owplConfigSetLocalHttpProxy",           PyOwplConfigSetLocalHttpProxy),
    PY_PHAPI_FUNCTION_DECL("owplConfigSetTunnel",                   PyOwplConfigSetTunnel),
    PY_PHAPI_FUNCTION_DECL("owplConfigSetNat",                      PyOwplConfigSetNat),
    PY_PHAPI_FUNCTION_DECL("owplConfigSetOutboundProxy",            PyOwplConfigSetOutboundProxy),
    PY_PHAPI_FUNCTION_DECL("owplConfigAddAudioCodecByName",         PyOwplConfigAddAudioCodecByName),
    PY_PHAPI_FUNCTION_DECL("owplConfigSetAudioCodecs",              PyOwplConfigSetAudioCodecs),
    PY_PHAPI_FUNCTION_DECL("owplConfigAddVideoCodecByName",         PyOwplConfigAddVideoCodecByName),
    PY_PHAPI_FUNCTION_DECL("owplConfigSetVideoCodecs",              PyOwplConfigSetVideoCodecs),
    PY_PHAPI_FUNCTION_DECL("owplConfigGetBoundLocalAddr",           PyOwplConfigGetBoundLocalAddr),
    PY_PHAPI_FUNCTION_DECL("owplConfigLocalHttpProxyGetAddr",       PyOwplConfigLocalHttpProxyGetAddr),
    PY_PHAPI_FUNCTION_DECL("owplConfigLocalHttpProxyGetPasswd",     PyOwplConfigLocalHttpProxyGetPasswd),
    PY_PHAPI_FUNCTION_DECL("owplConfigLocalHttpProxyGetPort",       PyOwplConfigLocalHttpProxyGetPort),
    PY_PHAPI_FUNCTION_DECL("owplConfigLocalHttpProxyGetUserName",   PyOwplConfigLocalHttpProxyGetUserName),
    PY_PHAPI_FUNCTION_DECL("owplAudioSetConfigString",              PyOwplAudioSetConfigString),
    PY_PHAPI_FUNCTION_DECL("owplLineAdd",                           PyOwplLineAdd),
    PY_PHAPI_FUNCTION_DECL("owplLineDelete",                        PyOwplLineDelete),
    PY_PHAPI_FUNCTION_DECL("owplLineGetProxy",                      PyOwplLineGetProxy),
    PY_PHAPI_FUNCTION_DECL("owplLineGetLocalUserName",              PyOwplLineGetLocalUserName),
    PY_PHAPI_FUNCTION_DECL("owplLineRegister",                      PyOwplLineRegister),
    PY_PHAPI_FUNCTION_DECL("owplLineSetOpts",                       PyOwplLineSetOpts),
    /*PY_PHAPI_FUNCTION_DECL("owplLineGetOpts",                     PyOwplLineGetOpts), // not yet implemented */
    PY_PHAPI_FUNCTION_DECL("owplLineGetUri",                        PyOwplLineGetUri),
    PY_PHAPI_FUNCTION_DECL("owplLineAddCredential",                 PyOwplLineAddCredential),
    PY_PHAPI_FUNCTION_DECL("owplLineSetBusy",                       PyOwplLineSetBusy),
    PY_PHAPI_FUNCTION_DECL("owplLineIsBusy",                        PyOwplLineIsBusy),
    PY_PHAPI_FUNCTION_DECL("owplCallCreate",                        PyOwplCallCreate),
    PY_PHAPI_FUNCTION_DECL("owplCallConnect",                       PyOwplCallConnect),
    PY_PHAPI_FUNCTION_DECL("owplCallConnectWithBody",               PyOwplCallConnectWithBody),
    PY_PHAPI_FUNCTION_DECL("owplCallAccept",                        PyOwplCallAccept),
    PY_PHAPI_FUNCTION_DECL("owplCallAnswer",                        PyOwplCallAnswer),
    PY_PHAPI_FUNCTION_DECL("owplCallAnswerWithBody",                PyOwplCallAnswerWithBody),
    PY_PHAPI_FUNCTION_DECL("owplCallReject",                        PyOwplCallReject),
    PY_PHAPI_FUNCTION_DECL("owplCallRejectWithPredefinedReason",    PyOwplCallRejectWithPredefinedReason),
    PY_PHAPI_FUNCTION_DECL("owplCallHold",                          PyOwplCallHold),
    PY_PHAPI_FUNCTION_DECL("owplCallHoldWithBody",                  PyOwplCallHoldWithBody),
    PY_PHAPI_FUNCTION_DECL("owplCallUnhold",                        PyOwplCallUnhold),
    PY_PHAPI_FUNCTION_DECL("owplCallUnholdWithBody",                PyOwplCallUnholdWithBody),
    PY_PHAPI_FUNCTION_DECL("owplCallDisconnect",                    PyOwplCallDisconnect),
    /*PY_PHAPI_FUNCTION_DECL("owplCallGetLocalID",                  PyOwplCallGetLocalID), // not yet implemented */
    /*PY_PHAPI_FUNCTION_DECL("owplCallGetRemoteID",                 PyOwplCallGetRemoteID), // not yet implemented */
    /*PY_PHAPI_FUNCTION_DECL("owplCallSetAssertedId",               PyOwplCallSetAssertedId), // not yet implemented */
    /*PY_PHAPI_FUNCTION_DECL("owplCallGetRemoteContact",            PyOwplCallGetRemoteContact), // not yet implemented */
    /*PY_PHAPI_FUNCTION_DECL("owplCallToneStart",                   PyOwplCallToneStart), // not yet implemented */
    /*PY_PHAPI_FUNCTION_DECL("owplCallToneStop",                    PyOwplCallToneStop), // not yet implemented */
    /*PY_PHAPI_FUNCTION_DECL("owplCallTonePlay",                    PyOwplCallTonePlay), // not yet implemented */
    /*PY_PHAPI_FUNCTION_DECL("owplCallAudioPlayFileStart",          PyOwplCallAudioPlayFileStart), // not yet implemented */
    /*PY_PHAPI_FUNCTION_DECL("owplCallAudioPlayFileStop",           PyOwplCallAudioPlayFileStop), // not yet implemented */
    /*PY_PHAPI_FUNCTION_DECL("owplCallSendInfo",                    PyOwplCallSendInfo), // not yet implemented */
    PY_PHAPI_FUNCTION_DECL("owplPresenceSubscribe",                 PyOwplPresenceSubscribe),
    PY_PHAPI_FUNCTION_DECL("owplPresenceUnsubscribe",               PyOwplPresenceUnsubscribe),
    PY_PHAPI_FUNCTION_DECL("owplPresencePublish",                   PyOwplPresencePublish),
    PY_PHAPI_FUNCTION_DECL("owplMessageSend",                       PyOwplMessageSend),
    PY_PHAPI_FUNCTION_DECL("owplMessageSendPlainText",              PyOwplMessageSendPlainText),
    PY_PHAPI_FUNCTION_DECL("owplMessageSendTypingState",            PyOwplMessageSendTypingState),
    PY_PHAPI_FUNCTION_DECL("owplMessageSendIcon",                   PyOwplMessageSendIcon),

    PY_PHAPI_FUNCTION_DECL_NULL,
};

/**
 * @brief Function called on import
 */
PyMODINIT_FUNC initpyphapi(void) {
#ifdef WIN32
    SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)unhandledExceptionFilter);
#endif
    Py_InitModule3("pyphapi", pyphapi_funcs, "");
    PyEval_InitThreads();
}
