#include "ddp_cmdq_sec.h"


static atomic_t gDebugSecSwCopy = ATOMIC_INIT(0);
static atomic_t gDebugSecCmdId = ATOMIC_INIT(0);

static DEFINE_MUTEX(gCmdqSecExecLock);       // lock to protect atomic secure task execution
static DEFINE_MUTEX(gCmdqSecContextLock);    // lock to protext atomic access gCmdqSecContextList
static struct list_head gCmdqSecContextList; // secure context list. note each porcess has its own sec context

// function declaretion
cmdqSecContextHandle cmdq_sec_context_handle_create(uint32_t tgid);

#if defined(__CMDQ_SECURE_PATH_SUPPORT__)
// secure path header
#include "tz_cmdq.h"

// the API for other code to acquire cmdq_mem session handle
// a NULL handle is returned when it fails
KREE_SESSION_HANDLE cmdq_session_handle(void)
{
    CMDQ_MSG("cmdq_session_handle() acquire TEE session\n");
    if (NULL == cmdq_session)
    {
    	TZ_RESULT ret;
        CMDQ_MSG("cmdq_session_handle() create session\n");
        
        ret = KREE_CreateSession(TZ_TA_CMDQ_UUID, &cmdq_session);
        if (ret != TZ_RESULT_SUCCESS)
        {
            CMDQ_ERR("cmdq_session_handle() failed to create session, ret=%d\n", ret);
            return NULL;
        }
    }

    CMDQ_MSG("cmdq_session_handle() session=%x\n", (unsigned int)cmdq_session);
    return cmdq_session;
}

KREE_SESSION_HANDLE cmdq_mem_session_handle(void)
{
    CMDQ_MSG("cmdq_mem_session_handle() acquires TEE memory session\n");
    if (NULL == cmdq_mem_session)
    {
    	TZ_RESULT ret;
        CMDQ_MSG("cmdq_mem_session_handle() create memory session\n");
        
        ret = KREE_CreateSession(TZ_TA_MEM_UUID, &cmdq_mem_session);
        if (ret != TZ_RESULT_SUCCESS)
        {
            CMDQ_ERR("cmdq_mem_session_handle() failed to create session: ret=%d\n", ret);
            return NULL;
        }
    }

    CMDQ_MSG("cmdq_mem_session_handle() session=%x\n", (unsigned int)cmdq_mem_session);
    return cmdq_mem_session;
}

int32_t cmdq_sec_setup_context_session(cmdqSecContextHandle handle)
{
    int32_t status = 0;

    // init iwc parameter

    // init secure session
    return status;
}

int32_t cmdq_sec_send_context_session_message(
            cmdqSecContextHandle handle,
            uint32_t iwcCommand,
            TaskStruct *pTask,
            int32_t thread,
            CmdqSecFillIwcCB iwcFillCB)
{
    int32_t status = 0;
    int32_t iwcRsp = 0;
    do
    {
        // fill message bufer

		MTEEC_PARAM param[4];
		unsigned int paramTypes;
		TZ_RESULT ret;
		KREE_SHAREDMEM_HANDLE cmdq_share_handle = 0;
		KREE_SHAREDMEM_PARAM  sharedParam;
		ret = KREE_RegisterSharedmem (cmdq_mem_session_handle(), &cmdq_share_handle, &sharedParam);
		if (ret != TZ_RESULT_SUCCESS)
		{
			CMDQ_ERR("cmdq register share memory Error: %d, line:%d, cmdq_mem_session(%x)", ret, __LINE__, (unsigned int)cmdq_mem_session_handle());
			return 0;
		}
		
		param[0].memref.handle = (uint32_t)cmdq_share_handle;

		paramTypes = TZ_ParamTypes2(TZPT_MEMREF_INPUT, TZPT_VALUE_INPUT);

		ret = KREE_TeeServiceCall(cmdq_session_handle(), TZCMD_CMDQ_SUBMIT_TASK, paramTypes, param);
		if(ret!= TZ_RESULT_SUCCESS)
		{
			CMDQ_ERR("TZCMD_CMDQ_SUBMIT_TASK fail, ret=%d \n", ret);
		}

		ret = KREE_UnregisterSharedmem(cmdq_mem_session_handle(), cmdq_share_handle);
		if (ret)
		{
			CMDQ_ERR("UREE_UnregisterSharedmem cmdq_share_handle Error: %d, line:%d, cmdq_mem_session(%x)", ret, __LINE__, (unsigned int)cmdq_mem_session_handle());
		}

        // send message

        // get secure task execution result

        // and then, update task state
        if(pTask)
        {
            pTask->taskState = (0 == iwcRsp)? (TASK_STATE_DONE): (TASK_STATE_ERROR);
        }

        // log print
        if(0 < status)
        {
            CMDQ_ERR("SEC_SEND: status[%d], cmdId[%d], iwcRsp[%d]\n", status, iwcCommand, iwcRsp);
        }
        else
        {
            CMDQ_MSG("SEC_SEND: status[%d], cmdId[%d], iwcRsp[%d]\n", status, iwcCommand, iwcRsp);
        }
    }while(0);

    return status;
}


int32_t cmdq_sec_submit_to_secure_world(
            uint32_t iwcCommand,
            TaskStruct *pTask,
            int32_t thread,
            CmdqSecFillIwcCB iwcFillCB)
{
    const bool skipSecCtxDump = (TZCMD_CMDQ_RES_RELEASE == iwcCommand) ? (true) : (false); // prevent nested lock gCmdqSecContextLock
    const int32_t tgid = current->tgid;
    cmdqSecContextHandle handle = NULL;
    int32_t status = 0;
    int32_t duration = 0;

    struct timeval tEntrySec;
    struct timeval tExitSec;

    mutex_lock(&gCmdqSecExecLock);
    smp_mb();

    CMDQ_MSG("[SEC]-->SEC_SUBMIT: tgid[%d]\n", tgid);
    do
    {
        // find handle first
        handle = cmdq_sec_find_context_handle_unlocked(tgid);
        if(NULL == handle)
        {
            CMDQ_ERR("SEC_SUBMIT: tgid %d err[NULL secCtxHandle]\n", tgid);
            status = -(CMDQ_ERR_NULL_SEC_CTX_HANDLE);
            break;
        }

        if(0 > cmdq_sec_setup_context_session(handle))
        {
            status = -(CMDQ_ERR_SEC_CTX_SETUP);
            break;
        }

        //
        // record profile data
        // tbase timer/time support is not enough currently,
        // so we treats entry/exit timing to secure world as the trigger/gotIRQ_&_wakeup timing
        //
        CMGQ_GET_CURRENT_TIME(tEntrySec);

        status = cmdq_sec_send_context_session_message(handle, iwcCommand, pTask, thread, iwcFillCB);

        CMGQ_GET_CURRENT_TIME(tExitSec);
        CMDQ_GET_TIME_DURATION(tEntrySec, tExitSec, duration);
        if(pTask)
        {
            pTask->trigger = tEntrySec;
            pTask->gotIRQ  = tExitSec;
            pTask->wakedUp = tExitSec;
        }

        // release resource

        // 
        // because secure world has no clkmgr support, delay reset HW when back to normal world
        // note do reset flow only if secure driver exec failed (i.e. result = -CMDQ_ERR_DR_EXEC_FAILED)
        //
        if((-CMDQ_ERR_TZ_EXEC_FAILED) == status)
        {
            cmdqResetHWEngine(pTask, thread);
        }
    }while(0);

    mutex_unlock(&gCmdqSecExecLock);

    if(0 > status)
    {
        if(!skipSecCtxDump)
        {
            mutex_lock(&gCmdqSecContextLock);
            cmdq_sec_dump_context_list();
            mutex_unlock(&gCmdqSecContextLock);
        }
        
        // throw AEE
        CMDQ_AEE("CMDQ", "[SEC]<--SEC_SUBMIT: err[%d], pTask[0x%p], THR[%d], tgid[%d], duration_ms[%d], cmdId[%d]\n", status, pTask, thread, tgid, duration, iwcCommand);
    }
    else
    {
        CMDQ_LOG("[SEC]<--SEC_SUBMIT: err[%d], pTask[0x%p], THR[%d], tgid[%d], duration_ms[%d], cmdId[%d]\n", status, pTask, thread, tgid, duration, iwcCommand);
    }    
    return status;
}


#endif

int32_t cmdq_exec_task_secure_with_retry(
    TaskStruct *pTask,
    int32_t thread,
    const uint32_t maxRetry)
{
#if defined(__CMDQ_SECURE_PATH_SUPPORT__)
    int32_t status = 0;
    int32_t i = 0;
    do
    {
        uint32_t commandId = cmdq_sec_get_commandId();
        commandId = (0 < commandId) ? (commandId) : (TZCMD_CMDQ_SUBMIT_TASK);

        status = cmdq_sec_submit_to_secure_world(commandId, pTask, thread, NULL);
        if(0 > status)
        {
            CMDQ_ERR("%s[%d]\n", __FUNCTION__, status);
            break;
        }
        i ++;
    }while(i < maxRetry);

    return status;

#else
    CMDQ_ERR("secure path not support\n");
    return -EFAULT;
#endif //__CMDQ_SECURE_PATH_SUPPORT__
}


//--------------------------------------------------------------------------------------------

cmdqSecContextHandle cmdq_sec_find_context_handle_unlocked(uint32_t tgid)
{
    cmdqSecContextHandle handle = NULL;

    do
    {
        struct cmdqSecContextStruct *secContextEntry = NULL;
        struct list_head *pos = NULL;
        list_for_each(pos, &gCmdqSecContextList)
        {
            secContextEntry = list_entry(pos, struct cmdqSecContextStruct, listEntry);
            if(secContextEntry && tgid == secContextEntry->tgid)
            {
                handle = secContextEntry;
                break;
            }
        }
    }while(0);

    CMDQ_MSG("SecCtxHandle_SEARCH: Handle[0x%p], tgid[%d]\n", handle, tgid);
    return handle;
}


int32_t cmdq_sec_release_context_handle_unlocked(cmdqSecContextHandle handle)
{
    int32_t status = 0;

    do
    {
        handle->referCount --;
        if(0 < handle->referCount)
        {
            break;
        }

        // when reference count <= 0, this secContext is not used. so, we should:
        // 1. clean up secure path in secure world
        #if defined(__CMDQ_SECURE_PATH_SUPPORT__)
        switch (handle->state)
        {
            case IWC_SES_ON_TRANSACTED:
            case IWC_SES_TRANSACTED:
                CMDQ_VERBOSE("SecCtxHandle_RELEASE: ask cmdqSecTl resource free\n");
                //cmdq_sec_submit_to_secure_world (TZCMD_CMDQ_RES_RELEASE, NULL, CMDQ_INVALID_THREAD, NULL);
                break;
            case IWC_SES_MSG_PACKAGED:
            case IWC_SES_OPENED:
            case IWC_WSM_ALLOCATED:
            case IWC_MOBICORE_OPENED:
            case IWC_INIT:
            default:
                CMDQ_VERBOSE("SecCtxHandle_RELEASE: no need cmdqSecTl resource free\n");
                break;
        }
        #endif

        // 2. delete secContext from list
        list_del(&(handle->listEntry));

        // 3. release secure path resource in normal world
        #if defined(__CMDQ_SECURE_PATH_SUPPORT__)
        {
            // because mc_open_session is too slow, we open once for each process context
            // and delay session clean up till process closes CMDQ device node
            //#if (CMDQ_OPEN_SESSION_ONCE)
            //cmdq_sec_teardown_context_session(handle);
            //#endif
        }
        #endif // defined(__CMDQ_SECURE_PATH_SUPPORT__)
    }while(0);
    return status;
}

int32_t cmdq_sec_release_context_handle(uint32_t tgid)
{
    int32_t status = 0;
    cmdqSecContextHandle handle = NULL;

    mutex_lock(&gCmdqSecContextLock);
    smp_mb();

    handle = cmdq_sec_find_context_handle_unlocked(tgid);
    if(handle)
    {
        CMDQ_MSG("SecCtxHandle_RELEASE: +tgid[%d], handle[0x%p], iwcState[%d]\n", tgid, handle, handle->state);
        status = cmdq_sec_release_context_handle_unlocked(handle);
        CMDQ_MSG("SecCtxHandle_RELEASE: -tgid[%d], status[%d]\n", tgid, status);
    }
    else
    {
        status = -1;
        CMDQ_ERR("SecCtxHandle_RELEASE: err[secCtxHandle not exist], tgid[%d]\n", tgid);
    }

    mutex_unlock(&gCmdqSecContextLock);
    return status;
}

cmdqSecContextHandle cmdq_sec_context_handle_create(uint32_t tgid)
{
    cmdqSecContextHandle handle = NULL;
    handle = kmalloc(sizeof(uint8_t*) * sizeof(cmdqSecContextStruct), GFP_ATOMIC);
    if (handle)
    {
        handle->state = IWC_INIT;
        handle->iwcMessage = NULL;

        handle->tgid = tgid;
        handle->referCount = 0;
    }
    else
    {
        CMDQ_ERR("SecCtxHandle_CREATE: err[LOW_MEM], tgid[%d]\n", tgid);
    }

    CMDQ_MSG("SecCtxHandle_CREATE: create new, Handle[0x%p], tgid[%d]\n", handle, tgid);
    return handle;
}

cmdqSecContextHandle cmdq_sec_acquire_context_handle(uint32_t tgid)
{
    cmdqSecContextHandle handle = NULL;

    mutex_lock(&gCmdqSecContextLock);
    smp_mb();
    do
    {
        // find sec context of a process
        handle = cmdq_sec_find_context_handle_unlocked(tgid);
        // if it dose not exist, create new one
        if(NULL == handle)
        {
            handle = cmdq_sec_context_handle_create(tgid);
            list_add_tail(&(handle->listEntry), &gCmdqSecContextList);
        }
    }while(0);

    // increase caller referCount
    if(handle)
    {
        handle->referCount ++;
    }

    CMDQ_MSG("[CMDQ]SecCtxHandle_ACQUIRE, Handle[0x%p], tgid[%d], refCount[%d]\n", handle, tgid, handle->referCount);
    mutex_unlock(&gCmdqSecContextLock);

    return handle;
}

void cmdq_sec_dump_context_list(void)
{
    struct cmdqSecContextStruct *secContextEntry = NULL;
    struct list_head *pos = NULL;

    CMDQ_ERR("=============== [CMDQ] sec context ===============\n");

    list_for_each(pos, &gCmdqSecContextList)
    {
        secContextEntry = list_entry(pos, struct cmdqSecContextStruct, listEntry);
        CMDQ_ERR("secCtxHandle[0x%p], tgid_%d[referCount: %d], state[%d], iwc[0x%p]\n",
            secContextEntry,
            secContextEntry->tgid,
            secContextEntry->referCount,
            secContextEntry->state,
            secContextEntry->iwcMessage);
    }
}

void cmdqSecDeInitialize(void)
{    
#if defined(__CMDQ_SECURE_PATH_SUPPORT__)
	// .TEE. close SVP CMDQ TEE serivice session
	// the sessions are created and accessed using [cmdq_session_handle()] / 
	// [cmdq_mem_session_handle()] API, and closed here.
	{
		TZ_RESULT ret;

		ret = KREE_CloseSession(cmdq_session);
		if(ret!=TZ_RESULT_SUCCESS)
		{
			CMDQ_ERR("DDP close ddp_session fail ret=%d \n", ret);
			return -1;
		}

		ret = KREE_CloseSession(cmdq_mem_session);
		if(ret!=TZ_RESULT_SUCCESS)
		{
			CMDQ_ERR("DDP close ddp_mem_session fail ret=%d \n", ret);
			return -1;
		}
	}	
#endif

    return;
}


void cmdqSecInitialize(void)
{
    INIT_LIST_HEAD(&gCmdqSecContextList);
}

//------------------------------------------------------------------------------------------
// debug
//
void cmdq_sec_set_commandId(uint32_t cmdId)
{
    atomic_set(&gDebugSecCmdId, cmdId);
}

const uint32_t cmdq_sec_get_commandId(void)
{
    return (uint32_t)(atomic_read(&gDebugSecCmdId));
}

void cmdq_debug_set_sw_copy(int32_t value)
{
    atomic_set(&gDebugSecSwCopy, value);
}

int32_t cmdq_debug_get_sw_copy(void)
{
    return atomic_read(&gDebugSecSwCopy);
}

