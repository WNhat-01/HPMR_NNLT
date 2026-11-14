/*
 * ============================================================================
 * MODULE 1: CORE LOGIC & SIMULATION ENGINE (circuit.c)
 * Phu trach: Thanh vien 1
 * Noi dung:
 *   - Quan ly cau truc du lieu bang mach (Components & Wires)
 *   - Tinh toan ham Boole cho 7 loai cong logic: NOT, AND, OR, XOR, NAND, NOR, XNOR
 *   - Thuat toan lan truyen tin hieu (Iterative Signal Propagation)
 * ============================================================================
 */

#include "../include/circuit.h"
#include <string.h>
#include <stdio.h>

static int EvalGateLogic(ComponentType type, int in0, int in1) {
    int a = (in0 != 0) ? 1 : 0;
    int b = (in1 != 0) ? 1 : 0;

    switch (type) {
        case COMP_NOT:  return !a;
        case COMP_AND:  return (a && b);
        case COMP_OR:   return (a || b);
        case COMP_XOR:  return (a ^ b);
        case COMP_NAND: return !(a && b);
        case COMP_NOR:  return !(a || b);
        case COMP_XNOR: return !(a ^ b);
        default:        return 0;
    }
}

void Circuit_Init(Circuit *circuit) {
    if (!circuit) return;
    memset(circuit, 0, sizeof(Circuit));
    for (int i = 0; i < MAX_COMPONENTS; i++) {
        circuit->components[i].id = i;
        circuit->components[i].active = false;
    }
}

int Circuit_AddComponent(Circuit *circuit, ComponentType type, float x, float y, bool locked, const char *label) {
    if (!circuit || type <= COMP_NONE || type >= COMP_TYPE_COUNT) return -1;

    for (int i = 0; i < MAX_COMPONENTS; i++) {
        if (!circuit->components[i].active) {
            Component *c = &circuit->components[i];
            memset(c, 0, sizeof(Component));
            c->active = true;
            c->id = i;
            c->type = type;
            c->x = x;
            c->y = y;
            c->width = 92.0f;
            c->height = 54.0f;
            c->locked = locked;

            if (label) {
                strncpy(c->label, label, sizeof(c->label) - 1);
            } else {
                strncpy(c->label, Circuit_GetTypeName(type), sizeof(c->label) - 1);
            }

            if (type == COMP_INPUT) {
                c->num_inputs = 0;
                c->num_outputs = 1;
                c->width = 84.0f;
                c->height = 48.0f;
                if (circuit->num_inputs < MAX_INPUTS) {
                    circuit->input_comp_ids[circuit->num_inputs++] = i;
                }
            } else if (type == COMP_OUTPUT) {
                c->num_inputs = 1;
                c->num_outputs = 0;
                c->width = 88.0f;
                c->height = 50.0f;
                if (circuit->num_outputs < MAX_OUTPUTS) {
                    circuit->output_comp_ids[circuit->num_outputs++] = i;
                }
            } else if (type == COMP_NOT) {
                c->num_inputs = 1;
                c->num_outputs = 1;
            } else {
                c->num_inputs = 2;
                c->num_outputs = 1;
            }

            Circuit_Simulate(circuit);
            return i;
        }
    }
    return -1;
}

bool Circuit_RemoveComponent(Circuit *circuit, int comp_id) {
    if (!circuit || comp_id < 0 || comp_id >= MAX_COMPONENTS) return false;
    Component *c = &circuit->components[comp_id];
    if (!c->active || c->locked) return false;

    /* Xoa tat ca day noi lien quan den linh kien nay */
    for (int w = 0; w < MAX_WIRES; w++) {
        if (circuit->wires[w].active) {
            if (circuit->wires[w].from_comp_id == comp_id ||
                circuit->wires[w].to_comp_id == comp_id) {
                circuit->wires[w].active = false;
            }
        }
    }

    c->active = false;
    Circuit_Simulate(circuit);
    return true;
}

bool Circuit_RemoveWireAtInput(Circuit *circuit, int to_comp_id, int to_pin_index) {
    if (!circuit) return false;
    bool removed = false;
    for (int w = 0; w < MAX_WIRES; w++) {
        if (circuit->wires[w].active &&
            circuit->wires[w].to_comp_id == to_comp_id &&
            circuit->wires[w].to_pin_index == to_pin_index) {
            circuit->wires[w].active = false;
            removed = true;
        }
    }
    if (removed) {
        Circuit_Simulate(circuit);
    }
    return removed;
}

bool Circuit_AddWire(Circuit *circuit, int from_comp_id, int to_comp_id, int to_pin_index) {
    if (!circuit) return false;
    if (from_comp_id < 0 || from_comp_id >= MAX_COMPONENTS) return false;
    if (to_comp_id < 0 || to_comp_id >= MAX_COMPONENTS) return false;
    if (from_comp_id == to_comp_id) return false; /* Khong cho phep tu noi vao chinh minh */

    Component *src = &circuit->components[from_comp_id];
    Component *dst = &circuit->components[to_comp_id];
    if (!src->active || !dst->active) return false;
    if (src->num_outputs == 0 || dst->num_inputs == 0) return false;
    if (to_pin_index < 0 || to_pin_index >= dst->num_inputs) return false;

    /* Moi chan Input chi nhan 1 day: xoa day cu neu da co */
    Circuit_RemoveWireAtInput(circuit, to_comp_id, to_pin_index);

    for (int w = 0; w < MAX_WIRES; w++) {
        if (!circuit->wires[w].active) {
            circuit->wires[w].active = true;
            circuit->wires[w].from_comp_id = from_comp_id;
            circuit->wires[w].to_comp_id = to_comp_id;
            circuit->wires[w].to_pin_index = to_pin_index;
            circuit->wires[w].signal_value = src->output_value;
            Circuit_Simulate(circuit);
            return true;
        }
    }
    return false;
}

void Circuit_ClearUserItems(Circuit *circuit) {
    if (!circuit) return;
    for (int w = 0; w < MAX_WIRES; w++) {
        circuit->wires[w].active = false;
    }
    for (int i = 0; i < MAX_COMPONENTS; i++) {
        if (circuit->components[i].active && !circuit->components[i].locked) {
            circuit->components[i].active = false;
        }
    }
    Circuit_Simulate(circuit);
}

void Circuit_Simulate(Circuit *circuit) {
    if (!circuit) return;

    /* Reset tat ca cong trung gian va dau ra truoc khi lan truyen */
    for (int i = 0; i < MAX_COMPONENTS; i++) {
        Component *c = &circuit->components[i];
        if (!c->active) continue;
        c->input_values[0] = 0;
        c->input_values[1] = 0;
        c->input_connected[0] = false;
        c->input_connected[1] = false;
        if (c->type != COMP_INPUT) {
            c->output_value = 0;
        }
    }

    /* Lan truyen tin hieu toi da MAX_COMPONENTS buoc de hoi tu */
    for (int step = 0; step < MAX_COMPONENTS; step++) {
        bool changed = false;

        /* 1. Cap nhat tin hieu tren day va dua vao chan Input */
        for (int w = 0; w < MAX_WIRES; w++) {
            Wire *wire = &circuit->wires[w];
            if (!wire->active) continue;

            Component *src = &circuit->components[wire->from_comp_id];
            Component *dst = &circuit->components[wire->to_comp_id];
            if (!src->active || !dst->active) continue;

            wire->signal_value = src->output_value;
            int pin = wire->to_pin_index;
            if (pin >= 0 && pin < 2) {
                dst->input_values[pin] = wire->signal_value;
                dst->input_connected[pin] = true;
            }
        }

        /* 2. Tinh toan dau ra cua tung linh kien */
        for (int i = 0; i < MAX_COMPONENTS; i++) {
            Component *c = &circuit->components[i];
            if (!c->active || c->type == COMP_INPUT) continue;

            int new_out = 0;
            if (c->type == COMP_OUTPUT) {
                new_out = c->input_connected[0] ? c->input_values[0] : 0;
            } else {
                new_out = EvalGateLogic(c->type, c->input_values[0], c->input_values[1]);
            }

            if (new_out != c->output_value) {
                c->output_value = new_out;
                changed = true;
            }
        }

        if (!changed && step > 1) break;
    }
}

int Circuit_CountUsedGates(const Circuit *circuit) {
    if (!circuit) return 0;
    int count = 0;
    for (int i = 0; i < MAX_COMPONENTS; i++) {
        if (circuit->components[i].active && !circuit->components[i].locked) {
            count++;
        }
    }
    return count;
}

int Circuit_CountGateByType(const Circuit *circuit, ComponentType type) {
    if (!circuit) return 0;
    int count = 0;
    for (int i = 0; i < MAX_COMPONENTS; i++) {
        if (circuit->components[i].active && !circuit->components[i].locked &&
            circuit->components[i].type == type) {
            count++;
        }
    }
    return count;
}

const char* Circuit_GetTypeName(ComponentType type) {
    switch (type) {
        case COMP_INPUT:  return "INPUT";
        case COMP_OUTPUT: return "OUTPUT";
        case COMP_NOT:    return "NOT";
        case COMP_AND:    return "AND";
        case COMP_OR:     return "OR";
        case COMP_XOR:    return "XOR";
        case COMP_NAND:   return "NAND";
        case COMP_NOR:    return "NOR";
        case COMP_XNOR:   return "XNOR";
        default:          return "NONE";
    }
}

const char* Circuit_GetTypeFormula(ComponentType type) {
    switch (type) {
        case COMP_NOT:    return "Y = ~A";
        case COMP_AND:    return "Y = A . B";
        case COMP_OR:     return "Y = A + B";
        case COMP_XOR:    return "Y = A ^ B";
        case COMP_NAND:   return "Y = ~(A . B)";
        case COMP_NOR:    return "Y = ~(A + B)";
        case COMP_XNOR:   return "Y = ~(A ^ B)";
        default:          return "";
    }
}
