#include "rtpport.h"

#ifdef TARGET_IS_HPUXKERNEL

#include <sys/types.h>
#include <sys/stream.h>
#include <sys/errno.h>
#include <sys/strstat.h>
#include <h/moddefs.h>

#include <h/param.h>
#ifndef CONVERGED_IO 
#include <sys/wsio.h> 
#else 
#include <wsio/wsio.h> 
#endif /* CONVERGED_IO */ 


#ifndef CM_SM
#define CM_SM "UNKNOWN"
#endif
#ifndef CM_WHAT_STRING
#define CM_WHAT_STRING "UNKNOWN WHAT STRING FOR " CM_SM
#endif
#pragma VERSIONID CM_WHAT_STRING

static char *cm_what_string=CM_WHAT_STRING;



static drv_info_t oRTP_info = {
    "oRTP",
    "FreeSoft",
    DRV_PSEUDO | DRV_MP_SAFE, 
    -1,
    -1,
    NULL, NULL, NULL
};

static drv_ops_t oRTP_ops = {
    NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL,
    C_ALLCLOSES | C_MAP_BUFFER_TO_KERNEL | C_MGR_IS_MP /* dflags */
};

static wsio_drv_data_t oRTP_data = {
    "oRTP",          /* char *drv_path */
    T_INTERFACE,    /* drv_type */
    DRV_CONVERGED,  /* drv_flags */
    NULL,           /* int (*drv_minor_build)() */
    NULL            /* int (*drv_minor_decode)() */
};

static wsio_drv_info_t oRTP_wsio_drv_info = {
    &oRTP_info,
    &oRTP_ops,
    &oRTP_data
};


#include <mod_conf.h>
#include <sys/moddefs.h>

/* Declare load and unload routine */
static int oRTP_load(void *arg);
static int oRTP_unload(void *drv_infop);

extern struct mod_operations gio_mod_ops;

/* The name of the struct mod_conf_data below MUST start
 * with the driver prefix defined in the 'master' file
 * else DLKM load errors occur.
 */

struct mod_conf_data oRTP_conf_data;

static struct mod_type_data oRTP_drv_link = {
    "Loadable ORTP",
    (void *)NULL
};

static struct modlink oRTP_mod_link[] = {
    { &gio_mod_ops, (void *)&oRTP_drv_link },
    { NULL, (void*)NULL }
};

/* The name of the struct modwrapper below MUST start
 * with the driver prefix defined in the 'master' file
 * else DLKM load will fail.
 */
struct modwrapper oRTP_wrapper = {
    MODREV,
    oRTP_load,
    oRTP_unload,
    (void (*)())NULL,
    (void*)&oRTP_conf_data,
    oRTP_mod_link
};



/***************************************************/
/* Load and unload routines only used in DLKM case */
/***************************************************/
static int oRTP_load(void *arg)
{
    int error = 0;
    /* Register driver with WSIO */
    oRTP_wsio_drv_info.drv_info = (drv_info_t*) arg;
    if (wsio_install_driver((void *)&oRTP_wsio_drv_info) == 0) {
      printf("oRTP not installed in kernel !\n");
      error=ENXIO;
    } 
	ortp_init();
    return(error);
}

static int oRTP_unload(void *drv_infop)
{
    int error = 0;
    int errnum;
    /* Unregister from WSIO */
    
    if((errnum=wsio_uninstall_driver(&oRTP_wsio_drv_info))!=0){
      printf("Error unloading oRTP.\n");
      error = ENXIO;
    }
    return(error);
}

#endif
