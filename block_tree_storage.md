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

*BlockTreeStoreError* is just a wrapper for trivial exception type which is derived from `std::runtime_error`. This is thrown on every failure in log apply path, inconsistent store state, while migration from leveldb -> flat file.

## CBlockFileInfo

```cpp
class CBlockFileInfo
{
public:
    uint32_t nBlocks{};      //!< number of blocks stored in file
    uint32_t nSize{};        //!< number of used bytes of block file
    uint32_t nUndoSize{};    //!< number of used bytes in the undo file
    uint32_t nHeightFirst{}; //!< lowest height of block in file
    uint32_t nHeightLast{};  //!< highest height of block in file
    uint64_t nTimeFirst{};   //!< earliest time of block in file
    uint64_t nTimeLast{};    //!< latest time of block in file

...

}
```
*CBlockFileInfo* is the per-block-file metadata record, one instance describes the contents of a single blkNNNNN.dat/revNNNNN.dat file pair on disk. *CBlockFileInfo* lets the components of Bitcoin Core know what's in each one without scanning the whole blkNNNNN.dat files. 

- blockfiles.dat <- stream of `CBlockFileInfo` records (via `BlockFileInfoWrapper`)
- headers.dat <- stream of `CDiskBlockIndex` records (via `DiskBlockIndexWrapper`)

*::SERIALIZE_METHODS* exists so that the migration code can still read the leveldb data whilst `BlockFileInfoWrapper` uses fixed-width encoding for the new flat file store:

```
struct BlockFileInfoWrapper : CBlockFileInfo {
    static constexpr size_t SERIALIZED_SIZE{36};

    BlockFileInfoWrapper() = default;

    explicit BlockFileInfoWrapper(const CBlockFileInfo* info) : CBlockFileInfo(*info)
    {
    }

    SERIALIZE_METHODS(BlockFileInfoWrapper, obj)
    {
        READWRITE(obj.nBlocks);
        READWRITE(obj.nSize);
        READWRITE(obj.nUndoSize);
        READWRITE(obj.nHeightFirst);
        READWRITE(obj.nHeightLast);
        READWRITE(obj.nTimeFirst);
        READWRITE(obj.nTimeLast);
    }
};
```
So we can see that *BlockFileInfoWrapper* is a wrapper for flat file store which is also derived from *CBlockFileInfo*.
`AddBlock()` method updated the stats for the block files.

**Summary**: `CBlockFileInfo` tracks how full each block/undo file is and what height/time range it covers, it gives summary about the block files.















