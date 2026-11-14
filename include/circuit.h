#ifndef CIRCUIT_H
#define CIRCUIT_H

#include <stdbool.h>

#define MAX_COMPONENTS 32
#define MAX_WIRES      64
#define MAX_INPUTS     4
#define MAX_OUTPUTS    3
#define MAX_PINS       3

/* Loai linh kien trong mach logic */
typedef enum {
    COMP_NONE = 0,
    COMP_INPUT,     /* Cong tac dau vao (Switch 0/1) */
    COMP_OUTPUT,    /* Den LED dau ra */
    COMP_NOT,       /* Cong dao NOT */
    COMP_AND,       /* Cong AND */
    COMP_OR,        /* Cong OR */
    COMP_XOR,       /* Cong XOR */
    COMP_NAND,      /* Cong NAND */
    COMP_NOR,       /* Cong NOR */
    COMP_XNOR,      /* Cong XNOR */
    COMP_TYPE_COUNT
} ComponentType;

/* Cau truc 1 linh kien tren bang mach */
typedef struct {
    bool active;
    int id;
    ComponentType type;
    float x, y;             /* Toa do tren bang mach (pixel) */
    float width, height;
    bool locked;            /* true neu la INPUT/OUTPUT co dinh cua man choi */
    char label[16];         /* Nhan hien thi: "A", "B", "OUT", "SUM", "CARRY"... */
    int num_inputs;         /* So chan dau vao (0, 1 hoac 2) */
    int num_outputs;        /* So chan dau ra (0 hoac 1) */
    int input_values[2];    /* Gia tri logic tai chan vao (0 hoac 1) */
    int output_value;       /* Gia tri logic tai chan ra (0 hoac 1) */
    bool input_connected[2];/* Chan vao da duoc noi day chua */
} Component;

/* Cau truc 1 soi day noi giua 2 linh kien */
typedef struct {
    bool active;
    int from_comp_id;       /* ID linh kien nguon (Output pin) */
    int to_comp_id;         /* ID linh kien dich (Input pin) */
    int to_pin_index;       /* Chi so chan vao cua linh kien dich (0 hoac 1) */
    int signal_value;       /* Muc logic tren day (0 hoac 1) */
} Wire;

/* Cau truc toan bo bang mach */
typedef struct {
    Component components[MAX_COMPONENTS];
    Wire wires[MAX_WIRES];
    int input_comp_ids[MAX_INPUTS];
    int num_inputs;
    int output_comp_ids[MAX_OUTPUTS];
    int num_outputs;
} Circuit;

/* API cho Thanh vien 1 (Core Logic & Simulation Engine) */
void Circuit_Init(Circuit *circuit);
int  Circuit_AddComponent(Circuit *circuit, ComponentType type, float x, float y, bool locked, const char *label);
bool Circuit_RemoveComponent(Circuit *circuit, int comp_id);
bool Circuit_AddWire(Circuit *circuit, int from_comp_id, int to_comp_id, int to_pin_index);
bool Circuit_RemoveWireAtInput(Circuit *circuit, int to_comp_id, int to_pin_index);
void Circuit_ClearUserItems(Circuit *circuit);

/* Tinh toan va lan truyen tin hieu logic tren toan mach */
void Circuit_Simulate(Circuit *circuit);

/* Dem so cong logic nguoi choi da su dung (khong tinh INPUT/OUTPUT co dinh) */
int  Circuit_CountUsedGates(const Circuit *circuit);
int  Circuit_CountGateByType(const Circuit *circuit, ComponentType type);

/* Helper lay ten va mau sac cua cong */
const char* Circuit_GetTypeName(ComponentType type);
const char* Circuit_GetTypeFormula(ComponentType type);

#endif /* CIRCUIT_H */
