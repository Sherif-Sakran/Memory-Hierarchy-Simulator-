#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include<sstream>
#include <bitset>
#include <cmath>
#include <cstdlib>
#include <ctime>
using namespace std;
struct cacheBlock {
    bool valid = 0;
    string tag;
};
struct cacheSet {
    vector<cacheBlock> blocks;
};

vector<cacheBlock> generateCache(int&, int&, int&, int&);
vector<cacheSet> generateCacheSets(int& s, int& l, int& c, int n, int& cacheAccessTime);
void accessSequenceDirectMapping(int, int, vector<cacheBlock>, int);
void missORhitDirectMapping(vector<cacheBlock>&, int, string, int&, int&);
void accessSequenceFullyAssociative(int l, vector<cacheBlock> cache, int cacheAccessTime);
void missORhitFullyAssociative(vector<cacheBlock>& cache, string tag, int& miss, int& hit);
void accessSequenceSetAssociative(int l, int c, int n, vector<cacheSet> cache, int cacheAccessTime);
void missORhitSetAssociative(vector<cacheSet>& cache, int index, int n, string tag, int& miss, int& hit);
//function to get the binary representation (as a string) of offset, index, and tag
vector<string> parseMemoryAddress(string, int, int, int);
int stToInt(string s);
int binaryToDecimal(int n);
void print(vector<cacheBlock>, int, int, int, int);
void printCacheSet(vector<cacheSet> cache, int n, int access, int hit, int miss, int cacheAccessTime);

//as per the project description, the miss penalty = 100 cycles
int missPenalty = 100;
int main() {
    int s, l, c, cacheAccessTime;
    int x = 0, n;
    cout << "Choose one of the following:\n1 for direct mapping\n2 for fully associative\n3 for n-associative\n";
    cin >> x;
    vector<cacheBlock> cacheBlocks;
    vector<cacheSet> cacheSets;
    if (x==1||x==2)
        cacheBlocks = generateCache(s, l, c, cacheAccessTime);
    if (x == 1)
        accessSequenceDirectMapping(l, c, cacheBlocks, cacheAccessTime);
    else if (x == 2)
        accessSequenceFullyAssociative(l, cacheBlocks, cacheAccessTime);
    else if (x == 3)
    {
        cout << "enter the associativity level\n";      
        cin >> n;
        cacheSets = generateCacheSets(s, l, c, n, cacheAccessTime);
        accessSequenceSetAssociative(l, c, n, cacheSets, cacheAccessTime);
    }


    return 0;
}

vector<cacheBlock> generateCache(int& s, int& l, int& c, int& cacheAccessTime) {
    ifstream myfile("Cache Information.txt");
    string instruction;

    if (!myfile.is_open())
        cout << "Unable to open file";
    else
    {
        getline(myfile, instruction);
        //cout << "s: " << instruction << endl;
        s = stToInt(instruction);
        getline(myfile, instruction);
        //cout << "l: " << instruction << endl;
        l = stToInt(instruction);
        c = s / l;
        getline(myfile, instruction);
        //cout << "cycles: " << instruction << endl;
        cacheAccessTime = stToInt(instruction);
    }
    vector<cacheBlock> cache(c);
    myfile.close();
    return cache;
}

void accessSequenceDirectMapping(int l, int c, vector<cacheBlock> cache, int cacheAccessTime) {
    int access = 0, miss = 0, hit = 0;
    int offsetSize = log2(l);
    int indexSize = log2(c);
    int tagSize = 32 - offsetSize - indexSize;
    //cout << offsetSize << "\t" << indexSize << "\t" << tagSize << endl;
    ifstream myfile("Access Sequence.txt");
    string instruction;

    if (!myfile.is_open())
        cout << "Unable to open file";
    else
    {
        while (getline(myfile, instruction))
        {
            int accessByteInt = stToInt(instruction);
            bitset<32> binary;
            binary = accessByteInt;
            string accessByteBinaryString = binary.to_string();
            //function to parse the binary representation (as a string) of tag, index, and offset respectivley
            vector<string> addressVector = parseMemoryAddress(accessByteBinaryString, offsetSize, indexSize, tagSize);
            string tag = addressVector[0];
            string indexString = addressVector[1];
            string offset = addressVector[2];
            int indexBinary= stToInt(indexString);
            int index = binaryToDecimal(indexBinary);
            //cout << "index: "<<index << endl;
            access++;
            missORhitDirectMapping(cache, index, tag, miss, hit);
            //cout << "access: " << access << endl;
            //cout << "miss: " << miss << endl;
            //cout << "hit: " << hit << endl;
            print(cache, access, hit, miss, cacheAccessTime);
        }
    }
    myfile.close();
}
void missORhitDirectMapping(vector<cacheBlock>& cache, int index, string tag, int& miss, int& hit) {
    //cout << "valid bit: " << cache[index].valid << endl << endl;
    if (cache[index].valid == 0)
    {
        miss++;
        cache[index].tag = tag;
        cache[index].valid = 1;
    }
    else
    {
        if (cache[index].tag == tag) //the required block
            hit++;
        else //not same block
        {
            miss++;
            cache[index].tag = tag;
        }
    }
}
vector<string> parseMemoryAddress(string address, int offsetSize, int indexSize, int tagSize) {
    //cout << "address: " << address << endl;
    vector<string> addressVector(3);
    addressVector[2] = address.substr(32 - offsetSize, offsetSize);
    addressVector[1] = address.substr(32 - offsetSize - indexSize, indexSize);
    addressVector[0] = address.substr(0, tagSize);
    /*
    addressVector[0] = tag;
    addressVector[1] = index;
    addressVector[2] = offset;
    */
    //cout << "address: " << tag << "\t"<< index << "\t" <<offset << endl;
    return addressVector;
}

int stToInt(string s) {
    int x;
    if (s.size() == 1)
    {
        if (s[0] >= '0' && s[0] <= '9')
            x = s[0] - '0';
        else
            x = s[0];
    }
    else
    {
        stringstream obj(s);
        obj >> x;
    }
    return x;
}
int binaryToDecimal(int n)
{
    int num = n;
    int decimal = 0;

    int base = 1;

    int temp = num;
    while (temp) {
        int last_digit = temp % 10;
        temp = temp / 10;

        decimal += last_digit * base;

        base = base * 2;
    }

    return decimal;
}



void print(vector<cacheBlock> cache, int access, int hit, int miss, int cacheAccessTime) {
/*
    • Valid bits and tags of all entries
    • Total number of accesses
    • The hit and miss ratios
    • The Average Memory Access Time (AMAT) of the memory hierarchy (in cycles)
    AMAT = Hit time + Miss rate × Miss penalty
*/
    cout << "index\tvalid bit\ttag\n";
    for (int i=0;i<cache.size();i++)
        cout << i <<"\t" <<cache[i].valid << "\t\t" << cache[i].tag << endl;

    cout << "Total number of accesses: " << access << endl;
    if (hit + miss != access)
        cout << "PROBLEM FOUND\n";
    cout << "Hit ratio: " << (1.0*hit)/access << endl;
    double missRate = (1.0 * miss) / access;
    cout << "Miss ratio: " << missRate << endl;
    cout << "AMAT: " << cacheAccessTime + missRate * missPenalty << endl;
    cout << endl << endl;
}
void accessSequenceFullyAssociative(int l, vector<cacheBlock> cache, int cacheAccessTime) {
    int access = 0, miss = 0, hit = 0;
    int offsetSize = log2(l);
    int tagSize = 32 - offsetSize;
    //cout << offsetSize << "\t" << indexSize << "\t" << tagSize << endl;
    ifstream myfile("Access Sequence.txt");
    string instruction;

    if (!myfile.is_open())
        cout << "Unable to open file";
    else
    {
        while (getline(myfile, instruction))
        {
            int accessByteInt = stToInt(instruction);
            bitset<32> binary;
            binary = accessByteInt;
            string accessByteBinaryString = binary.to_string();
            string tag = accessByteBinaryString.substr(0, tagSize);
            access++;
            missORhitFullyAssociative(cache, tag, miss, hit);
            //cout << "access: " << access << endl;
            //cout << "miss: " << miss << endl;
            //cout << "hit: " << hit << endl;
            print(cache, access, hit, miss, cacheAccessTime);
        }
    }
    myfile.close();
}



void missORhitFullyAssociative(vector<cacheBlock>& cache, string tag, int& miss, int& hit) {
    //cout << "valid bit: " << cache[index].valid << endl << endl;
    int i = 0;
    for (; i < cache.size(); i++)
        if (cache[i].valid == 1 && cache[i].tag == tag)
        {
            hit++;
            break;
        }
    //if (i!=cache.size()) //found, hit increased, and the loop breaked
    if (i == cache.size()) //not found -> miss
    {
        miss++;
        for (int i = 0; i < cache.size(); i++)
            if (cache[i].valid == 0)
            {
                cache[i].tag = tag;
                cache[i].valid = 1;
                return;
            }
    }
    //if the flow comes here, then the cache is full, and we need to replace. random replacement is applied.
    srand(time(0));
    int replaceIndex = rand() % cache.size(); //0->size - 1
    cache[replaceIndex].tag = tag;    
}

vector<cacheSet> generateCacheSets(int& s, int& l, int& c, int n, int& cacheAccessTime) {
    ifstream myfile("Cache Information.txt");
    string instruction;

    if (!myfile.is_open())
        cout << "Unable to open file";
    else
    {
        getline(myfile, instruction);
        //cout << "s: " << instruction << endl;
        s = stToInt(instruction);
        getline(myfile, instruction);
        //cout << "l: " << instruction << endl;
        l = stToInt(instruction);
        c = s / l;
        getline(myfile, instruction);
        //cout << "cycles: " << instruction << endl;
        cacheAccessTime = stToInt(instruction);
    }
    vector<cacheSet> cache(c/n);
    for (int i = 0; i < cache.size(); i++)
        cache[i].blocks.resize(n);    
    myfile.close();
    return cache;
}

void accessSequenceSetAssociative(int l, int c, int n, vector<cacheSet> cache, int cacheAccessTime) {
    int access = 0, miss = 0, hit = 0;
    int offsetSize = log2(l);
    int indexSize = log2(c/n);
    int tagSize = 32 - offsetSize - indexSize;
    //cout << offsetSize << "\t" << indexSize << "\t" << tagSize << endl;
    ifstream myfile("Access Sequence.txt");
    string instruction;

    if (!myfile.is_open())
        cout << "Unable to open file";
    else
    {
        while (getline(myfile, instruction))
        {
            int accessByteInt = stToInt(instruction);
            bitset<32> binary;
            binary = accessByteInt;
            string accessByteBinaryString = binary.to_string();
            //function to parse the binary representation (as a string) of tag, index, and offset respectivley
            vector<string> addressVector = parseMemoryAddress(accessByteBinaryString, offsetSize, indexSize, tagSize);
            string tag = addressVector[0];
            string indexString = addressVector[1];
            string offset = addressVector[2];
            int indexBinary = stToInt(indexString);
            int index = binaryToDecimal(indexBinary);
            if (c / n == 1)
                index = 0;
            //cout << "index: "<<index << endl;
            access++;
            missORhitSetAssociative(cache, index, n, tag, miss, hit);
            //cout << "access: " << access << endl;
            //cout << "miss: " << miss << endl;
            //cout << "hit: " << hit << endl;
            printCacheSet(cache,n, access, hit, miss, cacheAccessTime);
        }
    }
    myfile.close();
}

void missORhitSetAssociative(vector<cacheSet>& cache, int index, int n, string tag, int& miss, int& hit) {
    //cout << "valid bit: " << cache[index].valid << endl << endl;
    int i = 0;
    for (; i < n; i++) {
        if (cache[index].blocks[i].valid == 1 && cache[index].blocks[i].tag == tag)
        {
            hit++;
            break;
        }
    }
    //if (i!=cache.size()) //found, hit increased, and the loop breaked
    if (i == n) //not found -> miss
    {
        miss++;
        for (int i = 0; i < n; i++)
            if (cache[index].blocks[i].valid == 0)
            {
                cache[index].blocks[i].tag = tag;
                cache[index].blocks[i].valid = 1;
                return;
            }
    }
    //if the flow comes here, then the cache is full, and we need to replace. random replacement is applied.
    srand(time(0));
    int replaceIndex = rand() % n; //0-> n - 1
    cache[index].blocks[replaceIndex].tag = tag;
}

void printCacheSet(vector<cacheSet> cache, int n, int access, int hit, int miss, int cacheAccessTime) {
    for (int i = 0; i < cache.size(); i++)
    {
        cout << "set: " << i << endl;
        cout << "\tblock\tvalid bit\ttag\n";
        for (int j = 0; j < n; j++)
            cout << "\t" << j << "\t " <<cache[i].blocks[j].valid << "  \t\t" << cache[i].blocks[j].tag << endl;
    }
    cout << "Total number of accesses: " << access << endl;
    if (hit + miss != access)
        cout << "PROBLEM FOUND\n";
    cout << "Hit ratio: " << (1.0 * hit) / access << endl;
    double missRate = (1.0 * miss) / access;
    cout << "Miss ratio: " << missRate << endl;
    cout << "AMAT: " << cacheAccessTime + missRate * missPenalty << endl;
    cout << endl << endl;
}