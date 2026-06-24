# Block Tree Storage in Bitcoin Core

### Files in index directory
- **headers.dat**: ``<magic><version>`` then repeated `[DiskBlockIndexWrapper(104B)][crc32c(4B)]`. One record per block. `HEADER_FILE_MAGIC=0x1d5e2eb2`,
  `HEADER_FILE_VERSION=1`, data starts at offset `HEADER_FILE_DATA_START_POS=8`.
- **blockfiles.dat** — `<magic><version><last_block_file(4B)><crc32c>` then repeated `[BlockFileInfoWrapper(36B)][crc32c]`.
  One record per block file. `BLOCK_FILES_LAST_BLOCK_POS=8`, `BLOCK_FILES_DATA_START_POS=16`.
- **log.dat** — the write-ahead log (only present mid-write).
- **log_flag.dat** : The complete log write flag file.
- **reindex.dat / prune.dat** — boolean flags encoded by file existence.

## Helper/Wrapper enums and error handling
```cpp
enum class ValueType : uint8_t {
    BLOCK_FILE_INFO = 0,
    DISK_BLOCK_INDEX = 1,
};

class BlockTreeStoreError : public std::runtime_error
{
public:
    explicit BlockTreeStoreError(const std::string& msg) : std::runtime_error(msg) {}
};
```
*ValueType* is used to indicate that the type the store is built on. It tags two kinds of record.  `BLOCK_FILE_INFO` is used to make the blockfiles(blockfiles.dat),
`DISK_BLOCK_INDEX` -> `headers.dat`.

*BlockTreeStoreError* is just a wrapper for trivial exception type which is derived from `std::runtime_error`. This is thrown on every failure in log apply path,
inconsistent store state, while migration from leveldb -> flat file.

