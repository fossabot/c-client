#ifndef C_CLIENT_LDIAPI_H
#define C_CLIENT_LDIAPI_H

#include <stdbool.h>

#include "uthash.h"

#ifdef __cplusplus
#include <string>

extern "C" {
#endif

enum ld_log_level {
    LD_LOG_FATAL = 0,
    LD_LOG_CRITICAL,
    LD_LOG_ERROR,
    LD_LOG_WARNING,
    LD_LOG_INFO,
    LD_LOG_DEBUG,
    LD_LOG_TRACE
};

typedef enum {
    LDStatusInitializing = 0,
    LDStatusInitialized,
    LDStatusFailed,
    LDStatusShuttingdown,
    LDStatusShutdown
} LDStatus;

typedef enum {
    LDNodeNone = 0,
    LDNodeString,
    LDNodeNumber,
    LDNodeBool,
    LDNodeHash,
    LDNodeArray,
} LDNodeType;

typedef struct LDNode_i {
    union {
        char *key;
        unsigned int idx;
    };
    LDNodeType type;
    union {
        bool b;
        char *s;
        double n;
        struct LDNode_i *h;
        struct LDNode_i *a;
    };
    UT_hash_handle hh;
    int version;
    int variation;
    int flagversion;
    double track;
#ifdef __cplusplus
    struct LDNode_i *lookup(const std::string &key);
    struct LDNode_i *index(unsigned int idx);
#endif
} LDNode;

typedef struct LDConfig_i LDConfig;

typedef struct LDUser_i LDUser;

struct LDClient_i;

/* returns true on success */
bool LDSetString(char **target, const char *value);

LDConfig *LDConfigNew(const char *);
void LDConfigSetAllAttributesPrivate(LDConfig *config, bool allprivate);
void LDConfigSetBackgroundPollingIntervalMillis(LDConfig *config, int millis);
void LDConfigSetAppURI(LDConfig *config, const char *uri);
void LDConfigSetConnectionTimeoutMillies(LDConfig *config, int millis);
void LDConfigSetDisableBackgroundUpdating(LDConfig *config, bool disable);
void LDConfigSetEventsCapacity(LDConfig *config, int capacity);
void LDConfigSetEventsFlushIntervalMillis(LDConfig *config, int millis);
void LDConfigSetEventsURI(LDConfig *config, const char *uri);
void LDConfigSetMobileKey(LDConfig *config, const char *key);
void LDConfigSetOffline(LDConfig *config, bool offline);
void LDConfigSetStreaming(LDConfig *config, bool streaming);
void LDConfigSetPollingIntervalMillis(LDConfig *config, int millis);
void LDConfigSetStreamURI(LDConfig *config, const char *uri);
void LDConfigSetProxyURI(LDConfig *config, const char *uri);
void LDConfigSetUseReport(LDConfig *config, bool report);
void LDConfigAddPrivateAttribute(LDConfig *config, const char *name);

/* must have already called LDClientInit to receive valid client */
struct LDClient_i *LDClientGet();
/* create the global client singleton */
struct LDClient_i *LDClientInit(LDConfig *, LDUser *, unsigned int maxwaitmilli);

LDUser *LDUserNew(const char *);

void LDUserSetAnonymous(LDUser *user, bool anon);
void LDUserSetIP(LDUser *user, const char *str);
void LDUserSetFirstName(LDUser *user, const char *str);
void LDUserSetLastName(LDUser *user, const char *str);
void LDUserSetEmail(LDUser *user, const char *str);
void LDUserSetName(LDUser *user, const char *str);
void LDUserSetAvatar(LDUser *user, const char *str);
void LDUserSetSecondary(LDUser *user, const char *str);

bool LDUserSetCustomAttributesJSON(LDUser *user, const char *jstring);
void LDUserSetCustomAttributes(LDUser *user, LDNode *custom);
void LDUserAddPrivateAttribute(LDUser *user, const char *attribute);

char *LDClientSaveFlags(struct LDClient_i *client);
void LDClientRestoreFlags(struct LDClient_i *client, const char *data);

void LDClientIdentify(struct LDClient_i *, LDUser *);

void LDClientFlush(struct LDClient_i *client);
bool LDClientIsInitialized(struct LDClient_i *);
/* block until initialized up to timeout, returns true if initialized */
bool LDClientAwaitInitialized(struct LDClient_i *client, unsigned int timeoutmilli);
bool LDClientIsOffline(struct LDClient_i *);
void LDClientSetOffline(struct LDClient_i *);
void LDClientSetOnline(struct LDClient_i *);
void LDClientSetBackground(struct LDClient_i *client, bool background);
void LDClientClose(struct LDClient_i *);

void LDSetClientStatusCallback(void (callback)(int));

/* Access the flag store must unlock with LDClientUnlockFlags */
LDNode *LDClientGetLockedFlags(struct LDClient_i *client);
void LDClientUnlockFlags(struct LDClient_i *client);

void LDClientTrack(struct LDClient_i *client, const char *name);
void LDClientTrackData(struct LDClient_i *client, const char *name, LDNode *data);

/* returns a hash table of existing flags, must be freed with LDNodeFree */
LDNode *LDAllFlags(struct LDClient_i *const client);

bool LDBoolVariation(struct LDClient_i *, const char *, bool);
int LDIntVariation(struct LDClient_i *, const char *, int);
double LDDoubleVariation(struct LDClient_i *, const char *, double);
char *LDStringVariationAlloc(struct LDClient_i *, const char *, const char *);
char *LDStringVariation(struct LDClient_i *, const char *, const char *, char *, size_t);
LDNode *LDJSONVariation(struct LDClient_i *client, const char *key, const LDNode *);

void LDFree(void *);
void *LDAlloc(size_t amt);

/* functions for working with (JSON) nodes (aka hash tables) */
LDNode *LDNodeCreateHash(void);
LDNode * LDNodeAddBool(LDNode **hash, const char *key, bool b);
LDNode * LDNodeAddNumber(LDNode **hash, const char *key, double n);
LDNode * LDNodeAddString(LDNode **hash, const char *key, const char *s);
LDNode * LDNodeAddHash(LDNode **hash, const char *key, LDNode *h);
LDNode * LDNodeAddArray(LDNode **hash, const char *key, LDNode *a);
LDNode *LDNodeLookup(const LDNode *hash, const char *key);
void LDNodeFree(LDNode **hash);
unsigned int LDNodeCount(const LDNode *hash);
LDNode *LDCloneHash(const LDNode *original);
/* functions for treating nodes as arrays */
LDNode *LDNodeCreateArray(void);
LDNode * LDNodeAppendBool(LDNode **array, bool b);
LDNode * LDNodeAppendNumber(LDNode **array, double n);
LDNode * LDNodeAppendString(LDNode **array, const char *s);
LDNode *LDNodeIndex(const LDNode *array, unsigned int idx);
LDNode *LDCloneArray(const LDNode *original);
/* functions for converting nodes to / from JSON */
char *LDNodeToJSON(const LDNode* node);
LDNode *LDNodeFromJSON(const char *json);

void LDSetLogFunction(int userlevel, void (userlogfn)(const char *));

/*
 * store interface. open files, read/write strings, ...
 */
typedef bool (*LD_store_stringwriter)(void *context, const char *name, const char *data);
typedef char *(*LD_store_stringreader)(void *context, const char *name);

void LD_store_setfns(void *context, LD_store_stringwriter, LD_store_stringreader);

bool LD_store_filewrite(void *context, const char *name, const char *data);
char *LD_store_fileread(void *context, const char *name);

/*
 * listener function for flag changes.
 * arguments:
 * flag name
 * change type - 0 for new or updated, 1 for deleted
 */
typedef void (*LDlistenerfn)(const char *, int);
/*
 * register a new listener.
 */
void LDClientRegisterFeatureFlagListener(struct LDClient_i *, const char *, LDlistenerfn);
bool LDClientUnregisterFeatureFlagListener(struct LDClient_i *, const char *, LDlistenerfn);

#if !defined(__cplusplus) && !defined(LD_C_API)
typedef struct LDClient_i LDClient;
#endif

#ifdef __cplusplus
}

class LDClient {
    public:
        static LDClient *Get(void);
        static LDClient *Init(LDConfig *, LDUser *, unsigned int maxwaitmilli);

        bool isInitialized(void);
        bool awaitInitialized(unsigned int timeoutmilli);

        bool boolVariation(const std::string &, bool);
        int intVariation(const std::string &, int);
        double doubleVariation(const std::string &, double);
        std::string stringVariation(const std::string &, const std::string &);
        char *stringVariation(const std::string &, const std::string &, char *, size_t);

        LDNode *JSONVariation(const std::string &, const LDNode *);

        LDNode *getLockedFlags();
        void unlockFlags();

        LDNode *getAllFlags();

        void setOffline();
        void setOnline();
        bool isOffline();
        void setBackground(bool background);

        std::string saveFlags();
        void restoreFlags(const std::string &);

        void identify(LDUser *);

        void track(const std::string &name);
        void track(const std::string &name, LDNode *data);

        void flush(void);
        void close(void);

        void registerFeatureFlagListener(const std::string &name, LDlistenerfn fn);
        bool unregisterFeatureFlagListener(const std::string &name, LDlistenerfn fn);

    private:
        struct LDClient_i *client;
};

#endif

#endif /* C_CLIENT_LDIAPI_H */
