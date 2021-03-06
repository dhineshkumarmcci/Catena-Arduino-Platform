/*

Module:  Catena_CommandStream.h

Function:
        class McciCatena::cCommandStream

Copyright notice:
        See accompanying LICENSE file.

Author:
        Terry Moore, MCCI Corporation	March 2017

*/

#ifndef _CATENA_COMMANDSTREAM_H_		/* prevent multiple includes */
#define _CATENA_COMMANDSTREAM_H_

#ifndef _CATENA_POLLABLEINTERFACE_H_
# include "Catena_PollableInterface.h"
#endif

#ifndef _CATENA_STREAMLINECOLLECTOR_H_
# include "Catena_StreamLineCollector.h"
#endif

#ifndef _CATENABASE_TYPES_H_
# include "CatenaBase_types.h"
#endif

namespace McciCatena {

class cCommandStream;	// forward


class cCommandStream : public cPollableObject
        {
public:
        cCommandStream() {};
        virtual ~cCommandStream() {};

        // neither copyable nor movable.
        cCommandStream(const cCommandStream&) = delete;
        cCommandStream& operator=(const cCommandStream&) = delete;
        cCommandStream(const cCommandStream&&) = delete;
        cCommandStream& operator=(const cCommandStream&&) = delete;

        bool begin(
                cStreamLineCollector *pCollector, // the command line reader
                CatenaBase *pCatena		  // the owning Catena object
                );

        virtual void poll(void);

        enum CommandStatus : int
                {
                kUnknown = -2,
                kIoError = -1,
                kSuccess = 0,
                kError = 1,
                kInvalidParameter = 2,
                kInternalError = 3,
                kNotInitialized = 4,
                kReadError = 5,
                kWriteError = 6,
                kCreateError = 7,
                kPending = 8,
                };

        typedef CommandStatus (CommandFn)(
                cCommandStream *pThis,
                void *pContext,
                int argc,
                char **argv
                );

        struct cEntry
                {
                const char *pName;
                CommandFn *pDispatch;
                };

        class cDispatch;

        void registerCommands(cDispatch *pDispatch, void *pContext);
        void printf(const char *pFmt, ...)
                __attribute__((__format__(__printf__, 2, 3)));
                /* format counts start with 2 for non-static C++ member fns */

        // convert parameter from argument vector to uint32_t.
        static CommandStatus getuint32(
                int argc,
                char **argv,
                int iArg,
                unsigned radix,
                uint32_t& result,
                uint32_t uDefault
                );

        // complete an asynchronous command.
        void completeCommand(CommandStatus status);

        // get the echo-enable state of the underlying stream.
        bool getEcho() const
                {
                return this->m_pCollector->getEcho();
                }

        // set the echo-enable state of the underlying stream.
        void setEcho(bool fEnable)
                {
                this->m_pCollector->setEcho(fEnable);
                }

protected:

private:
        // launch the read request
        void launchRead(void);

        // a simple callback that calls the method.
        static void readCompleteCb(
                void *pCtx,
                cStreamLineCollector::ErrorCode uStatus,
                uint8_t *pBuffer,
                size_t nBuffer
                )
                {
                cCommandStream * const pThis =
                        static_cast<cCommandStream *>(pCtx);

                pThis->readComplete(uStatus, pBuffer, nBuffer);
                }

        // indicate that a line has arrived from the lower level.
        void readComplete(
                cStreamLineCollector::ErrorCode uStatus,
                uint8_t *pBuffer,
                size_t nBuffer
                );

        // parse and dispatch the current command
        int parseAndDispatch();

        // dispatch a command
        int dispatch(int argc, char **argv);

        // dispatch help
        int dispatchHelp(int argc, char **argv);

        // the command buffer
        uint8_t m_buffer[128];

        // the status of the operation

        size_t m_nRead;			// how many bytes were read
        bool m_fReadPending = false;	// is a read pending?
        bool m_fReadComplete = false;	// is a read complete?
        uint8_t m_ReadStatus;		// status of last read.
        bool m_fCmdActive = false;       // is a command being processed.
	CommandStatus m_CmdStatus;	// status of this operation.

        // the argument vector
        char *(m_argv[sizeof(m_buffer)/4]);

        // the collector
        cStreamLineCollector *m_pCollector = nullptr;

        // pointer to the owning Catena
        CatenaBase *m_pCatena = nullptr;

        // pointer to the dispatch tables
        cDispatch *m_pHead = nullptr;
        };

class cCommandStream::cDispatch
        {
public:
        cDispatch(
                const cEntry *pEntries,
                unsigned sizeofEntries,
                const char *pGroupName = nullptr
                )
                {
                this->m_pEntries = pEntries;
                this->m_nEntries = sizeofEntries / sizeof(pEntries[0]);
                this->m_pGroupName = pGroupName;
                };
        ~cDispatch() {};

        // neither copyable nor movable.
        cDispatch(const cDispatch&) = delete;
        cDispatch& operator=(const cDispatch&) = delete;
        cDispatch(const cDispatch&&) = delete;
        cDispatch& operator=(const cDispatch&&) = delete;

        // dispatch a command into this table.
        int dispatch(int argc, char **argv);

private:
        // look up a command in this dispatch table, return pointer
        // or nullptr.
        const cEntry *search(const char *pCommand) const;

        friend class cCommandStream;
        cDispatch *m_pNext = nullptr;
        cDispatch *m_pLast = nullptr;
        const cEntry *m_pEntries = nullptr;
        unsigned m_nEntries = 0;
        const char *m_pGroupName = nullptr;
        void *m_pContext;
        };

}; // namespace McciCatena

/**** end of Catena_CommandStream.h ****/
#endif /* _CATENA_COMMANDSTREAM_H_ */
