#define EXPORT __attribute__((used)) __attribute__ ((visibility ("default")))
#define NULL ((void*)0)

typedef char  int8_t;
typedef short int16_t;
typedef int   int32_t;
typedef long  int64_t;

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;
typedef unsigned long  uint64_t;

typedef struct {
    uint8_t fpower;
    uint8_t forward;
    uint8_t bpower;
    uint8_t backward;
} memory_header;

uint8_t find_power(int number, int limit) {
    uint8_t power = 0;
    while (number / (1<<power) >= limit) {
        power++;
    }
    return power;
}

int memory_forward(memory_header *header) {
    return header->forward * (1<<(header->fpower & 0x7f));
}

int memory_backward(memory_header *header) {
    return header->backward * (1<<(header->bpower & 0x7f));
}

EXPORT void* malloc(unsigned long size) {
    memory_header* header = (memory_header*)((void*)8);
    if (header->forward == 0) {
        uint8_t power = find_power(STACK_START - 8, 128);
        header->fpower = power;
        header->forward = (uint8_t)((STACK_START - 8) / (1<<power));
        header->bpower = 1<<7;
        header->backward = 0;
    }
    if (size == 0) {
        return NULL;
    }
    while (1) {
        uint8_t allocated = header->fpower >> 7;
        int mem = memory_forward(header);

        if (!allocated && mem >= size) {
            break;
        }

        header = (memory_header*)((int)header + sizeof(memory_header) + mem);
        if ((int)header >= STACK_START - 8) {
            return NULL;
        }
    }
    uint8_t power = find_power(size, 128);
    uint8_t newsize = (uint8_t)(size / (1<<power));

    int memsize = memory_forward(header);
    if (memsize - newsize > sizeof(memory_header)) {
        memory_header *newheader = (memory_header*)((int)header + sizeof(memory_header) + newsize);
        int newfree = memsize - sizeof(memory_header) - newsize;
        uint8_t newpower = find_power(newfree, 128);
        newheader->fpower = newpower;
        newheader->forward = (uint8_t)(newfree / (1<<newpower));
        newheader->bpower = (uint8_t)(1<<7 | power);
        newheader->backward = newsize;
    }

    header->fpower = (uint8_t)(1<<7 | power);
    header->forward = newsize;
    header->bpower &= 0x7f;
    return (void*)((int)header + sizeof(memory_header));
}

EXPORT void free(void *ptr) {
    if (ptr == NULL) {
        return;
    }
    memory_header* header = (memory_header*)((int)ptr - sizeof(memory_header));

    header->fpower &= 0x7f;

    while (1) {
        memory_header *forward = (memory_header *) ((int) header + sizeof(memory_header) + memory_forward(header));
        if (forward->bpower >> 7 || forward->fpower >> 7) {
            break;
        }
        int mem = memory_forward(header) + memory_forward(forward) + sizeof(memory_header);
        header->fpower = find_power(mem, 128);
        header->forward = (uint8_t)(mem / (1<<header->fpower));
    }

    while (1) {
        int back = memory_backward(header);
        if (back == 0) {
            break;
        }
        int mem = memory_forward(header) + sizeof(memory_header) + back;
        header = (memory_header*)((int)header - sizeof(memory_header) - back);
        if (header->fpower >> 7) {
            break;
        }
        header->fpower = find_power(mem, 128);
        header->forward = (uint8_t)(mem / (1<<header->fpower));
    }
}

EXPORT void* foo(int n) {
    char *m = (char*)malloc(n);
    for (int i = 0; i < n; i ++) {
        m[i] = (char)(i+1);
    }
    return m;
}