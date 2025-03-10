const MAX_TIME_STAMP_DIFF = 10 * 60 * 1000
const NONCE_CACHE_TIME = MAX_TIME_STAMP_DIFF * 2
const TIMESTAMP_LEN = 13
const COLLECTION_NAME = 'kms_appid_info'
const CMK_EFFECTIVE_DURATION = 1 * 365 * 24 * 60 * 60 * 1000
const CMK_LOOP_CLEAR_TIME = 24 * 60 * 60 * 1000
const CMK_EXPIRE_TIME_EXPAND = 10 * 24 * 60 * 60 * 1000
const CMK_LOOP_CLEAR_EXECUTION_TIME = 3
const SM_SECRET_VERSION_STAGE_CURRENT = 1
const SM_SECRET_VERSION_STAGE_PREVIOUS = 0
const SECRETNAME_LENGTH_MAX = 64
const SECRETDATA_LENGTH_MAX = 4 * 1024
const DESCRIPTION_LENGTH_MAX = 4 * 1024
const DEFAULT_DELETE_RECOVERY_DAYS = 30
const QUOTE_LENGTH_MAX = 10924 // the length after 8K base64
const NONCE_LENGTH_MAX = 4 * 1024
const CHALLENGE_LENGTH_MAX = 4 * 1024
const UUID_LENGTH = 36
const MR_ENCLAVE_LENGTH = 64
const MR_SIGNER_LENGTH = 64

module.exports = {
  MAX_TIME_STAMP_DIFF,
  NONCE_CACHE_TIME,
  TIMESTAMP_LEN,
  COLLECTION_NAME,
  CMK_EFFECTIVE_DURATION,
  CMK_LOOP_CLEAR_TIME,
  CMK_EXPIRE_TIME_EXPAND,
  CMK_LOOP_CLEAR_EXECUTION_TIME,
  SM_SECRET_VERSION_STAGE_CURRENT,
  SM_SECRET_VERSION_STAGE_PREVIOUS,
  SECRETNAME_LENGTH_MAX,
  SECRETDATA_LENGTH_MAX,
  DESCRIPTION_LENGTH_MAX,
  DEFAULT_DELETE_RECOVERY_DAYS,
  QUOTE_LENGTH_MAX,
  NONCE_LENGTH_MAX,
  CHALLENGE_LENGTH_MAX,
  UUID_LENGTH,
  MR_ENCLAVE_LENGTH,
  MR_SIGNER_LENGTH
}
