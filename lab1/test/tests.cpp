// #include <gtest/gtest.h>
// #include <gmock/gmock.h>
// #include <memory>

// #include <hashtable.hpp>

// class HashTableManager {
// public:
//     explicit HashTableManager()
//     {
//         m_table = std::make_unique<HashTable>();
//         m_record.fields.push_back("Яблоко");
//         m_record.fields.push_back("34");
//         m_record.fields.push_back("54");
//     }
//     ~HashTableManager() = default;

//     bool testAddMethod()
//     {
//         return m_table->add(m_record);
//     }
//     bool testRemoveMethod()
//     {
//         auto key = m_record.fields[0];
//         return m_table->remove(key);
//     }
//     int testFindMethod()
//     {
//         auto key = m_record.fields[0];
//         return m_table->find(key);
//     }
//     bool testEditMethod(const Record& rec)
//     {
//         return m_table->modify(rec.fields);
//     }

//     Record getRecord() const
//     {
//         return m_record;
//     }
//     std::vector<Record> getHashTableRecords() const
//     {
//         return m_table->getData();
//     }

// private:
//     std::unique_ptr<HashTable> m_table;
//     Record m_record;

// };


// TEST(TestHashTableCollisions, WhenAddingData)
// {   
//     auto manager = std::make_unique<HashTableManager>();
//     auto result = manager->testAddMethod();
//     ASSERT_TRUE(result);
// }
// TEST(TestHashTableCollisions, WhenFindingData)
// {   
//     auto manager = std::make_unique<HashTableManager>();
//     auto result = manager->testAddMethod();
//     ASSERT_TRUE(result);

//     auto index = manager->testFindMethod();
//     result = index != -1;
//     ASSERT_TRUE(result);
// }
// TEST(TestHashTable, WhenRemovingData)
// {   
//     auto manager = std::make_unique<HashTableManager>();
//     auto result = manager->testAddMethod();
//     ASSERT_TRUE(result);

//     result = manager->testRemoveMethod();
//     ASSERT_TRUE(result);
// }
// TEST(TestHashTable, WhenModifyingData)
// {   
//     auto manager = std::make_unique<HashTableManager>();
//     auto result = manager->testAddMethod();
//     ASSERT_TRUE(result);

//     auto record = manager->getRecord();
//     Record newRecord;
//     auto key = record.fields[0];
//     newRecord.fields.push_back(key);
//     newRecord.fields.push_back("1");
//     newRecord.fields.push_back("1");
//     result = manager->testEditMethod(newRecord);
//     ASSERT_TRUE(result);

//     record = manager->getHashTableRecords()[0];
//     ASSERT_TRUE(record.fields.size() == newRecord.fields.size());
// }