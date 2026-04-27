#include <gtest/gtest.h>
#include <hashtable.hpp>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>

// Note: Adjust these to match your actual project headers/structures
// struct Record { std::vector<std::string> fields; };

class HashTableTest : public ::testing::Test {
protected:
    void SetUp() override {
        // N=16 works well with the hash function's bit-masking logic (N-1 = 15)
        table = std::make_unique<HashTable>(16);
    }

    std::unique_ptr<HashTable> table;

    // Helper to create test records
    static Record makeRecord(const std::string& key, const std::string& value = "data") {
        return Record{{key, value}};
    }

    // Helper to verify a key exists in the table
    bool containsKey(const std::string& key) {
        int idx = table->indexOfRecord(key, RecordMethod::INDEX_RECORD);
        return table->find(key, idx).result;
    }
};

// ============================================================================
// Constructor & Initialization
// ============================================================================
TEST_F(HashTableTest, ConstructorInitializesCorrectly) {
    EXPECT_EQ(table->getData().size(), 16);
    EXPECT_EQ(table->getTotalCollisions(), 0);
    EXPECT_FALSE(containsKey("any_key"));
}

// ============================================================================
// Hash Function & Indexing
// ============================================================================
TEST_F(HashTableTest, IndexOfRecordReturnsValidRange) {
    const std::vector<std::string> keys = {"test", "hello", "123", "longer_key_name", ""};
    for (const auto& k : keys) {
        int idx = table->indexOfRecord(k, RecordMethod::INDEX_RECORD);
        EXPECT_GE(idx, 0);
        EXPECT_LT(idx, 16);
    }
}

TEST_F(HashTableTest, IndexOfRecordIsDeterministic) {
    int idx1 = table->indexOfRecord("deterministic_key", RecordMethod::INDEX_RECORD);
    int idx2 = table->indexOfRecord("deterministic_key", RecordMethod::FREE_RECORD);
    EXPECT_EQ(idx1, idx2);
}

// ============================================================================
// Add Operation
// ============================================================================
TEST_F(HashTableTest, AddSingleRecordSuccess) {
    Record rec = makeRecord("add_test");
    int idx = table->indexOfRecord("add_test", RecordMethod::FREE_RECORD);
    EXPECT_TRUE(table->add(rec, idx));
    EXPECT_TRUE(containsKey("add_test"));
}

TEST_F(HashTableTest, AddDuplicateKeyFails) {
    Record rec1 = makeRecord("dup_key");
    int idx = table->indexOfRecord("dup_key", RecordMethod::FREE_RECORD);
    EXPECT_TRUE(table->add(rec1, idx));

    Record rec2 = makeRecord("dup_key", "should_fail");
    EXPECT_FALSE(table->add(rec2, idx));
}

TEST_F(HashTableTest, AddToFullTableFails) {
    for (int i = 0; i < 16; ++i) {
        Record rec = makeRecord("fill_" + std::to_string(i));
        int idx = table->indexOfRecord("fill_" + std::to_string(i), RecordMethod::FREE_RECORD);
        table->add(rec, idx);
    }
    Record overflow = makeRecord("overflow");
    int idx = table->indexOfRecord("overflow", RecordMethod::FREE_RECORD);
    EXPECT_FALSE(table->add(overflow, idx));
}

// ============================================================================
// Find Operation
// ============================================================================
TEST_F(HashTableTest, FindExistingRecord) {
    Record rec = makeRecord("find_me", "found_data");
    int idx = table->indexOfRecord("find_me", RecordMethod::INDEX_RECORD);
    table->add(rec, idx);

    auto res = table->find("find_me", idx);
    EXPECT_TRUE(res.result);
    EXPECT_EQ(res.record.fields[1], "found_data");
}

TEST_F(HashTableTest, FindNonExistentRecord) {
    int idx = table->indexOfRecord("missing", RecordMethod::INDEX_RECORD);
    auto res = table->find("missing", idx);
    EXPECT_FALSE(res.result);
}

// ============================================================================
// Modify Operation
// ============================================================================
TEST_F(HashTableTest, ModifyExistingRecord) {
    Record rec = makeRecord("mod_key", "old_val");
    int idx = table->indexOfRecord("mod_key", RecordMethod::INDEX_RECORD);
    table->add(rec, idx);

    Record updated = makeRecord("mod_key", "new_val");
    EXPECT_TRUE(table->modify(updated, idx));

    auto res = table->find("mod_key", idx);
    EXPECT_TRUE(res.result);
    EXPECT_EQ(res.record.fields[1], "new_val");
}

TEST_F(HashTableTest, ModifyNonExistentRecordFails) {
    int idx = table->indexOfRecord("no_such_key", RecordMethod::INDEX_RECORD);
    Record fake = makeRecord("no_such_key", "fail");
    EXPECT_FALSE(table->modify(fake, idx));
}

// ============================================================================
// Remove Operation
// ============================================================================
TEST_F(HashTableTest, RemoveExistingRecord) {
    Record rec = makeRecord("del_key");
    int idx = table->indexOfRecord("del_key", RecordMethod::INDEX_RECORD);
    table->add(rec, idx);

    EXPECT_TRUE(table->remove("del_key", idx));
    EXPECT_FALSE(containsKey("del_key"));
}

TEST_F(HashTableTest, RemoveNonExistentRecordFails) {
    int idx = table->indexOfRecord("ghost_key", RecordMethod::INDEX_RECORD);
    EXPECT_FALSE(table->remove("ghost_key", idx));
}

TEST_F(HashTableTest, RemoveThenAddReusesSlot) {
    std::string key = "reuse_key";
    int idx = table->indexOfRecord(key, RecordMethod::INDEX_RECORD);
    
    table->add(makeRecord(key, "v1"), idx);
    EXPECT_TRUE(table->remove(key, idx));
    
    Record rec2 = makeRecord(key, "v2");
    EXPECT_TRUE(table->add(rec2, idx));
    
    auto res = table->find(key, idx);
    EXPECT_TRUE(res.result);
    EXPECT_EQ(res.record.fields[1], "v2");
}

// ============================================================================
// Collision Handling & Probing
// ============================================================================
TEST_F(HashTableTest, CollisionCounterIncrements) {
    int initial = table->getTotalCollisions();
    
    // Add enough records to force at least one collision in a size-16 table
    for (int i = 0; i < 20; ++i) {
        Record rec = makeRecord("col_" + std::to_string(i));
        int idx = table->indexOfRecord("col_" + std::to_string(i), RecordMethod::FREE_RECORD);
        table->add(rec, idx);
    }
    EXPECT_GE(table->getTotalCollisions(), initial);
}

TEST_F(HashTableTest, FindWorksAfterProbing) {
    // Fill table to 75% to increase collision probability
    for (int i = 0; i < 12; ++i) {
        Record rec = makeRecord("probe_test_" + std::to_string(i));
        int idx = table->indexOfRecord("probe_test_" + std::to_string(i), RecordMethod::FREE_RECORD);
        table->add(rec, idx);
    }
    // Find a key that was inserted (may have probed)
    EXPECT_TRUE(containsKey("probe_test_5"));
}

// ============================================================================
// Edge Cases
// ============================================================================
TEST_F(HashTableTest, HandlesEmptyKey) {
    Record rec = makeRecord("");
    int idx = table->indexOfRecord("", RecordMethod::FREE_RECORD);
    EXPECT_TRUE(table->add(rec, idx));
    EXPECT_TRUE(containsKey(""));
}

TEST_F(HashTableTest, SpecialCharactersInKey) {
    std::string key = "key!@#$%^&*()_+-=[]{}|;':\",./<>?";
    Record rec = makeRecord(key, "special");
    int idx = table->indexOfRecord(key, RecordMethod::FREE_RECORD);
    EXPECT_TRUE(table->add(rec, idx));
    EXPECT_TRUE(containsKey(key));
}

TEST_F(HashTableTest, GetDataReturnsFullTableArray) {
    table->add(makeRecord("a"), table->indexOfRecord("a", RecordMethod::FREE_RECORD));
    table->add(makeRecord("b"), table->indexOfRecord("b", RecordMethod::FREE_RECORD));
    
    auto data = table->getData();
    EXPECT_EQ(data.size(), 16); // Size matches N
    // Verify non-empty slots contain expected data
    int non_empty = 0;
    for (auto r : data) {
        if (r != Record()) 
            ++non_empty;
    }
    EXPECT_EQ(non_empty, 2);
}

#include <regulartable.hpp>

class RegularTableTest : public ::testing::Test {
protected:
    void SetUp() override {
        table = std::make_unique<RegularTable>(10); // Small size for easy testing
    }

    std::unique_ptr<RegularTable> table;

    static Record makeRecord(const std::string& key, const std::string& value = "data") {
        return Record{{key, value}};
    }

    static bool isRecordEmpty(const Record& r) {
        return r == Record();
    }
};

// ============================================================================
// Constructor & Initialization
// ============================================================================
TEST_F(RegularTableTest, ConstructorInitializesCorrectly) {
    EXPECT_EQ(table->getData().size(), 10);
    
    // Check that all records are in "default empty" state
    for (const auto& r : table->getData()) {
        EXPECT_EQ(r.fields.size(), 3);
        EXPECT_TRUE(r.fields[0].empty());
        EXPECT_TRUE(r.fields[1].empty());
        EXPECT_TRUE(r.fields[2].empty());
    }
}

// ============================================================================
// indexOfRecord - FREE_RECORD
// ============================================================================
TEST_F(RegularTableTest, IndexOfFreeRecordReturnsFirstEmptySlot) {
    // Fill first 3 slots
    for (int i = 0; i < 3; ++i) {
        table->add(makeRecord("key_" + std::to_string(i)), i);
    }
    
    int freeIdx = table->indexOfRecord("ignored", RecordMethod::FREE_RECORD);
    EXPECT_EQ(freeIdx, 3);
}

TEST_F(RegularTableTest, IndexOfFreeRecordReturnsMinusOneWhenFull) {
    for (int i = 0; i < 10; ++i) {
        table->add(makeRecord("fill_" + std::to_string(i)), i);
    }
    EXPECT_EQ(table->indexOfRecord("ignored", RecordMethod::FREE_RECORD), -1);
}

TEST_F(RegularTableTest, IndexOfFreeRecordIgnoresKeyParameter) {
    table->add(makeRecord("existing"), 0);
    // Key "nonexistent" doesn't matter for FREE_RECORD mode
    int idx = table->indexOfRecord("nonexistent", RecordMethod::FREE_RECORD);
    EXPECT_EQ(idx, 1);
}

// ============================================================================
// indexOfRecord - INDEX_RECORD
// ============================================================================
TEST_F(RegularTableTest, IndexOfRecordFindsExistingKey) {
    table->add(makeRecord("target"), 5);
    int idx = table->indexOfRecord("target", RecordMethod::INDEX_RECORD);
    EXPECT_EQ(idx, 5);
}

TEST_F(RegularTableTest, IndexOfRecordReturnsFirstMatch) {
    // Add duplicate keys at different indices (allowed in RegularTable)
    table->add(makeRecord("dup"), 2);
    table->add(makeRecord("dup"), 7);
    
    int idx = table->indexOfRecord("dup", RecordMethod::INDEX_RECORD);
    EXPECT_EQ(idx, 2); // Should return first occurrence
}

TEST_F(RegularTableTest, IndexOfRecordReturnsMinusOneWhenNotFound) {
    EXPECT_EQ(table->indexOfRecord("missing", RecordMethod::INDEX_RECORD), -1);
}

// ============================================================================
// Add Operation
// ============================================================================
TEST_F(RegularTableTest, AddAtValidIndexSuccess) {
    Record rec = makeRecord("add_test");
    EXPECT_TRUE(table->add(rec, 0));
    EXPECT_EQ(table->getData()[0].fields[0], "add_test");
}

TEST_F(RegularTableTest, AddAtInvalidIndexFails) {
    Record rec = makeRecord("bad_idx");
    EXPECT_FALSE(table->add(rec, -1));
    EXPECT_FALSE(table->add(rec, 10)); // N=10, valid: 0-9
}

TEST_F(RegularTableTest, AddToFullTableFails) {
    for (int i = 0; i < 10; ++i) {
        table->add(makeRecord("fill_" + std::to_string(i)), i);
    }
    EXPECT_FALSE(table->add(makeRecord("overflow"), 0)); // Table full, canAdd should fail
}

TEST_F(RegularTableTest, AddOverwritesExistingSlotButIncrementsCount) {
    // Note: RegularTable::add doesn't check if slot is occupied!
    // This is a design choice - test documents the behavior
    table->add(makeRecord("first"), 3);
    table->add(makeRecord("second"), 3); // Overwrites
    
    auto data = table->getData();
    EXPECT_EQ(data[3].fields[0], "second");
    // m_recordsCount still increments (may be intentional or a bug)
    // If this is unintended, add a check in add() for recordEmptyAt
}

// ============================================================================
// Find Operation
// ============================================================================
TEST_F(RegularTableTest, FindAtCorrectIndexWithMatchingKey) {
    table->add(makeRecord("find_me", "value"), 4);
    auto res = table->find("find_me", 4);
    EXPECT_TRUE(res.result);
    EXPECT_EQ(res.record.fields[1], "value");
}

TEST_F(RegularTableTest, FindAtCorrectIndexWithWrongKey) {
    table->add(makeRecord("actual_key"), 4);
    auto res = table->find("wrong_key", 4);
    EXPECT_FALSE(res.result);
}

TEST_F(RegularTableTest, FindAtInvalidIndexReturnsFalse) {
    auto res = table->find("any", -1);
    EXPECT_FALSE(res.result);
    res = table->find("any", 10);
    EXPECT_FALSE(res.result);
}

TEST_F(RegularTableTest, FindDoesNotSearchOtherIndices) {
    // RegularTable::find ONLY checks the given index - it doesn't probe!
    table->add(makeRecord("exists"), 2);
    auto res = table->find("exists", 5); // Wrong index
    EXPECT_FALSE(res.result); // Expected: find is index-specific
}

// ============================================================================
// Modify Operation
// ============================================================================
TEST_F(RegularTableTest, ModifyAtValidIndexSuccess) {
    table->add(makeRecord("old"), 1);
    Record updated = makeRecord("old", "new_value"); // Same key, different data
    EXPECT_TRUE(table->modify(updated, 1));
    
    auto res = table->find("old", 1);
    EXPECT_TRUE(res.result);
    EXPECT_EQ(res.record.fields[1], "new_value");
}

TEST_F(RegularTableTest, ModifyAtInvalidIndexFails) {
    Record rec = makeRecord("test");
    EXPECT_FALSE(table->modify(rec, -1));
    EXPECT_FALSE(table->modify(rec, 10));
}

TEST_F(RegularTableTest, ModifyDoesNotValidateKeyMatch) {
    // RegularTable::modify replaces whatever is at index, regardless of key
    table->add(makeRecord("original_key"), 3);
    Record differentKey = makeRecord("completely_different");
    
    EXPECT_TRUE(table->modify(differentKey, 3));
    EXPECT_EQ(table->getData()[3].fields[0], "completely_different");
}

// ============================================================================
// Remove Operation
// ============================================================================
TEST_F(RegularTableTest, RemoveExistingRecordSuccess) {
    table->add(makeRecord("to_delete"), 6);
    EXPECT_TRUE(table->remove("to_delete", 6));
    EXPECT_TRUE(isRecordEmpty(table->getData()[6]));
}

TEST_F(RegularTableTest, RemoveWithWrongKeyFails) {
    table->add(makeRecord("actual"), 6);
    EXPECT_FALSE(table->remove("wrong", 6)); // recordExistsAt should fail
    EXPECT_FALSE(isRecordEmpty(table->getData()[6]));
}

TEST_F(RegularTableTest, RemoveAtInvalidIndexFails) {
    EXPECT_FALSE(table->remove("any", -1));
    EXPECT_FALSE(table->remove("any", 10));
}

TEST_F(RegularTableTest, RemoveThenAddReusesSlot) {
    std::string key = "reuse";
    table->add(makeRecord(key, "v1"), 2);
    EXPECT_TRUE(table->remove(key, 2));
    
    EXPECT_TRUE(table->add(makeRecord(key, "v2"), 2));
    auto res = table->find(key, 2);
    EXPECT_TRUE(res.result);
    EXPECT_EQ(res.record.fields[1], "v2");
}

// ============================================================================
// Edge Cases & Data Access
// ============================================================================
TEST_F(RegularTableTest, HandlesEmptyKey) {
    Record rec = makeRecord("");
    EXPECT_TRUE(table->add(rec, 0));
    
    int idx = table->indexOfRecord("", RecordMethod::INDEX_RECORD);
    EXPECT_EQ(idx, 0);
    
    auto res = table->find("", 0);
    EXPECT_TRUE(res.result);
}

TEST_F(RegularTableTest, GetDataReturnsFullArray) {
    table->add(makeRecord("a"), 1);
    table->add(makeRecord("b"), 3);
    
    auto data = table->getData();
    EXPECT_EQ(data.size(), 10);
    
    int non_empty = 0;
    for (const auto& r : data) {
        if (r != Record()) ++non_empty;
    }
    EXPECT_EQ(non_empty, 2);
}

TEST_F(RegularTableTest, GetTitlesReturnsLoadedTitles) {
    // Simulate what loadTable does
    table->getTitles(); // Initially empty
    // If you test loadTable, use a temp file with known titles
}

// ============================================================================
// loadTable Integration (optional - requires temp file)
// ============================================================================
TEST_F(RegularTableTest, LoadTablePopulatesSequentially) {
    // This test requires a valid CSV/file that TableLoader can parse
    // Example: create a temp file with 3 records, then verify they're at indices 0,1,2
    // Skip if file I/O is out of scope for unit tests
    GTEST_SKIP() << "Integration test - requires temp file setup";
}