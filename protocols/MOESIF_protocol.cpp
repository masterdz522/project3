#include "MOESIF_protocol.h"
#include "../sim/mreq.h"
#include "../sim/sim.h"
#include "../sim/hash_table.h"

extern Simulator *Sim;

/*************************
 * Constructor/Destructor.
 *************************/
MOESIF_protocol::MOESIF_protocol (Hash_table *my_table, Hash_entry *my_entry)
    : Protocol (my_table, my_entry)
{
    state = MOESIF_CACHE_I;
}

MOESIF_protocol::~MOESIF_protocol ()
{    
}

void MOESIF_protocol::dump (void)
{
    const char *block_states[12] = {"X","I","S","E","O","M","F","ISE","IM","FM","SM","OM"};
    fprintf (stderr, "MOESIF_protocol - state: %s\n", block_states[state]);
}

void MOESIF_protocol::process_cache_request (Mreq *request)
{
	switch (state) {
        case MOESIF_CACHE_I: do_cache_I(request); break;
        case MOESIF_CACHE_S: do_cache_S(request); break;
        case MOESIF_CACHE_E: do_cache_E(request); break;
        case MOESIF_CACHE_O: do_cache_O(request); break;
        case MOESIF_CACHE_M: do_cache_M(request); break;
        case MOESIF_CACHE_F: do_cache_F(request); break;

    default:
        fatal_error ("Invalid Cache State for MOESIF Protocol\n");
    }
}

void MOESIF_protocol::process_snoop_request (Mreq *request)
{
    //this->dump();
	switch (state) {
        case MOESIF_CACHE_I: do_snoop_I(request); break;
        case MOESIF_CACHE_S: do_snoop_S(request); break;
        case MOESIF_CACHE_E: do_snoop_E(request); break;
        case MOESIF_CACHE_O: do_snoop_O(request); break;
        case MOESIF_CACHE_M: do_snoop_M(request); break;
        case MOESIF_CACHE_F: do_snoop_F(request); break;
        case MOESIF_CACHE_ISE: do_snoop_ISE(request); break;
        case MOESIF_CACHE_IM: do_snoop_IM(request); break;
        case MOESIF_CACHE_FM: do_snoop_FM(request); break;
        case MOESIF_CACHE_SM: do_snoop_SM(request); break;
        case MOESIF_CACHE_OM: do_snoop_OM(request); break;                        
    default:
    	fatal_error ("Invalid Cache State for MOESIF Protocol\n");
    }
    //this->dump();
}

inline void MOESIF_protocol::do_cache_F (Mreq *request)
{
switch(request->msg){
    case LOAD:
        send_DATA_to_proc(request->addr);
        break;
    case STORE:
        send_GETM(request->addr);
        state = MOESIF_CACHE_FM;
        Sim->cache_misses++;
        break;
    default:
        request->print_msg(my_table->moduleID,"ERROR");
        fatal_error("Client: F state shouldn't see this message\n");
}
}

inline void MOESIF_protocol::do_cache_I (Mreq *request)
{
switch(request->msg){
    case LOAD:
        send_GETS(request->addr);
        state = MOESIF_CACHE_ISE;
        Sim->cache_misses++;
        break;
    case STORE:
        send_GETM(request->addr);
        state = MOESIF_CACHE_IM;
        Sim->cache_misses++;
        break;
    default:
        request->print_msg(my_table->moduleID,"ERROR");
        fatal_error("Client: I state shouldn't see this message\n");
}
}

inline void MOESIF_protocol::do_cache_S (Mreq *request)
{
switch(request->msg){
    case LOAD:
        send_DATA_to_proc(request->addr);
        break;
    case STORE:
        send_GETM(request->addr);
        state=MOESIF_CACHE_SM;
        Sim->cache_misses++;
        break;
    default:
        request->print_msg(my_table->moduleID,"ERROR");
        fatal_error("Client: S state shouldn't see this message\n");
}
}

inline void MOESIF_protocol::do_cache_E (Mreq *request)
{
switch(request->msg){
    case LOAD:
        send_DATA_to_proc(request->addr);
        break;
    case STORE:
        state = MOESIF_CACHE_M;
        Sim->silent_upgrades++;
        send_DATA_to_proc(request->addr);
        break;
    default:
        request->print_msg(my_table->moduleID,"ERROR");
        fatal_error("Client: E state shouldn't see this message\n");
}
}

inline void MOESIF_protocol::do_cache_O (Mreq *request)
{
switch(request->msg){
    case LOAD:
        send_DATA_to_proc(request->addr);
        break;
    case STORE:
        send_GETM(request->addr);
        state=MOESIF_CACHE_OM;
        Sim->cache_misses++;
        break;
    default:
        request->print_msg(my_table->moduleID,"ERROR");
        fatal_error("Client: O state shouldn't see this message\n");
}
}

inline void MOESIF_protocol::do_cache_M (Mreq *request)
{
switch(request->msg){
    case LOAD:
    case STORE:
        send_DATA_to_proc(request->addr);
        break;
    default:
        request->print_msg(my_table->moduleID,"ERROR");
        fatal_error("Client: M state shouldn't see this message\n");
}
}

inline void MOESIF_protocol::do_snoop_F (Mreq *request)
{
    switch(request->msg){
        case GETS:
            set_shared_line();
            send_DATA_on_bus(request->addr,request->src_mid);
            break;
        case GETM:
            send_DATA_on_bus(request->addr,request->src_mid);
            state=MOESIF_CACHE_I;
            break;
        case DATA:
            break;
        default:
            request->print_msg (my_table->moduleID,"ERROR");
            fatal_error("Client: F state shouldn't see this message\n");
    }
}

inline void MOESIF_protocol::do_snoop_I (Mreq *request)
{
    switch(request->msg){
        case GETS:
            break;
        case GETM:
            break;
        case DATA:
            break;
        default:
            request->print_msg (my_table->moduleID,"ERROR");
            fatal_error("Client: I state shouldn't see this message\n");
    }
}

inline void MOESIF_protocol::do_snoop_S (Mreq *request)
{
    switch(request->msg){
        case GETS:
            set_shared_line();
            break;
        case GETM:
            state=MOESIF_CACHE_I;
            break;
        case DATA:
            break;
        default:
            request->print_msg (my_table->moduleID,"ERROR");
            fatal_error("Client: S state shouldn't see this message\n");
    }
}

inline void MOESIF_protocol::do_snoop_E (Mreq *request)
{
    switch(request->msg){
        case GETS:
            set_shared_line();
            send_DATA_on_bus(request->addr,request->src_mid);
            state=MOESIF_CACHE_F;
            break;
        case GETM:
            set_shared_line();
            send_DATA_on_bus(request->addr,request->src_mid);
            state=MOESIF_CACHE_I;
            break;
        case DATA:
            break;
        default:
            request->print_msg (my_table->moduleID,"ERROR");
            fatal_error("Client: E state shouldn't see this message\n");
    }
}

inline void MOESIF_protocol::do_snoop_O (Mreq *request)
{
    switch(request->msg){
        case GETS:
            set_shared_line();
            send_DATA_on_bus(request->addr,request->src_mid);
            break;
        case GETM:
            set_shared_line();
            send_DATA_on_bus(request->addr,request->src_mid);
            state=MOESIF_CACHE_I;
            break;
        case DATA:
            break;
        default:
            request->print_msg (my_table->moduleID,"ERROR");
            fatal_error("Client: O state shouldn't see this message\n");
    }
}

inline void MOESIF_protocol::do_snoop_M (Mreq *request)
{
    switch(request->msg){
        case GETS:
            set_shared_line();
            send_DATA_on_bus(request->addr,request->src_mid);
            state=MOESIF_CACHE_O;
            break;
        case GETM:
            if(!get_shared_line()){
                send_DATA_on_bus(request->addr,request->src_mid);
            }
            set_shared_line();
            state=MOESIF_CACHE_I;
            break;
        case DATA:
            break;
        default:
            request->print_msg (my_table->moduleID,"ERROR");
            fatal_error("Client: M state shouldn't see this message\n");
    }
}

inline void MOESIF_protocol::do_snoop_ISE (Mreq *request)
{
    switch(request->msg){
        case GETS:
            break;
        case GETM:
            break;
        case DATA:
            send_DATA_to_proc(request->addr);
            if(get_shared_line()){
                state=MOESIF_CACHE_S;
            }
            else state=MOESIF_CACHE_E;
            break;
        default:
            request->print_msg (my_table->moduleID,"ERROR");
            fatal_error("Client: ISE state shouldn't see this message\n");
    }
}

inline void MOESIF_protocol::do_snoop_IM (Mreq *request)
{
    switch(request->msg){
        case GETS:
            break;
        case GETM:
            break;
        case DATA:
            send_DATA_to_proc(request->addr);
            state=MOESIF_CACHE_M;
            break;
        default:
            request->print_msg (my_table->moduleID,"ERROR");
            fatal_error("Client: IM state shouldn't see this message\n");
    }
}

inline void MOESIF_protocol::do_snoop_FM (Mreq *request)
{
    switch(request->msg){
        case GETS:
            //if(!get_shared_line()){
                send_DATA_on_bus(request->addr,request->src_mid);
            //}
            set_shared_line();
            break;
        case GETM:
            if(!get_shared_line()){
                send_DATA_on_bus(request->addr,request->src_mid);
            }
            set_shared_line();
            break;
        case DATA:
            send_DATA_to_proc(request->addr);
            state=MOESIF_CACHE_M;
            break;
        default:
            request->print_msg (my_table->moduleID,"ERROR");
            fatal_error("Client: FM state shouldn't see this message\n");
    }
}


inline void MOESIF_protocol::do_snoop_SM (Mreq *request)
{
    switch(request->msg){
        case GETS:
            set_shared_line();
            break;
        case GETM:
            break;
        case DATA:
            send_DATA_to_proc(request->addr);
            state=MOESIF_CACHE_M;
            break;
        default:
            request->print_msg (my_table->moduleID,"ERROR");
            fatal_error("Client: SM state shouldn't see this message\n");
    }
}

inline void MOESIF_protocol::do_snoop_OM (Mreq *request)
{
    switch(request->msg){
        case GETS:
            //if(!get_shared_line()){
                send_DATA_on_bus(request->addr,request->src_mid);
            //}
            set_shared_line();
            break;
        case GETM:
            if(!get_shared_line()){
                send_DATA_on_bus(request->addr,request->src_mid);
            }
            set_shared_line();
            break;
        case DATA:
            send_DATA_to_proc(request->addr);
            state=MOESIF_CACHE_M;
            break;
        default:
            request->print_msg (my_table->moduleID,"ERROR");
            fatal_error("Client: OM state shouldn't see this message\n");
    }
}
