# Bitcoin Core UTXO data structure explanation, coins.h/cpp

Let's first start with lifecycle of UTXO journey. Bitcoin Core stores an output of a transaction in `CTxOut` data structure. 

When someone creates a Bitcoin transaction, they specify outputs using `CTxOut` class which contains two things `nValue`(the amount in satoshis)
`scriptPubkey`(locking script for spending the output). When transaction is validated and gets mined, this is where the transformation happens, the validation 
function records this new UTXO in the UTXO set but it can't just store the `CTxOut` in the database as it needs some additional context such as whether it came 
from a coinbase transaction, at what height(`nHeight` in coins.h ) it was created. So for this reason we have a separate data structure for UTXO's in Bitcoin Core.

### coins.h
`fCoinBase`(bit flag to identify coinbase txn), `nHeight`(bit flag for height records). Serialize member functions serializes data. Instead of using a full 
32 bits for `fCoinBase`(which only needs to be true or false), it uses just one bit. The remaining 31 bits of that same 32-bit word store the height. 
This is a memory optimization—Bitcoin needs to track millions of UTXOs, so every byte counts and vice-versa.

The Coin has several constructors and a `Clear()` method that marks it as spent by setting the output to null. 
The `IsSpent()` method checks if the output is null. Notice the serialization methods—Bitcoin needs to save these coins to disk in a compact format, 
so it encodes the height and coinbase flag together as a single variable-length integer. Similarly `IsCoinBase()` returns const `fCoinBase` flag to identify whether
it came from a coinbase txn.

### CCoinsCacheEntry

`using CoinsCachePair = std::pair<const COutPoint, CCoinsCacheEntry>;` a type alias for caching coins. Bitcoin needs to track 
which UTXOs have been modified in a cache so that when it flushes changes to the parent cache, it doesn't have to scan through potentially millions of 
entries. The `CCoinsCacheEntry` accomplishes this by maintaining a doubly-linked list of only the modified entries, allowing the system to iterate through 
just the changes rather than the entire dataset. The type `CoinsCachePair` is defined as `std::pair<const COutPoint`, `CCoinsCacheEntry>`, which is 
what the unordered map stores. So these pointers don't just point to other entries, they point to complete key-value pairs in the map. 
This is necessary because when iterating the linked list, you need access to both the outpoint (the key) and the entry (the value).

```bash
enum Flags {
    DIRTY = (1 << 0),   // Binary: 01
    FRESH = (1 << 1),   // Binary: 10
};
```
These are bit flags for setting the type of UTXO/Coin(DIRTY/FRESH. etc). See more in comment section of the code(coins.h).

```bash
static void AddFlags(uint8_t flags, CoinsCachePair& pair, 
                     CoinsCachePair& sentinel) noexcept
```
This method adds flags to an entry and inserts it into the linked list if it wasn't already there. The linked-list is circular linked-list, the algorithm in the
method adds the coin node to the linked-list. The `Assume` checks you are adding a meaningful flag(FRESH or DIRTY). `SetClean()` removes the entry of coin node from
the doubly linked-list. 

### CCoinsMap: Core data structure
<!--TODO-->

























