#include "World.h"
#include "Base.h"
#include "Payload.h"
#include <iostream>
using std::cout;

World::~World() {
    
}

bool World::remove_payload(const string &id) {
    
}

void World::elapse(int time) {

}

void World::process_arrived_payloads() {
    
}

// Given. You do not need to modify this function.
bool World::recall_payload(const string &id) {
    if (m_payloads.find(id) == m_payloads.end()) return false; // payload does not exist

    // back up the payload
    Payload *p = m_payloads[id];
    remove_payload(id); // this function should not cause the payload to actually get deleted

    // Re-add the items to the receiver before create_payload deducts them from the new sender.
    *m_bases[p->get_receiver_id()] += *p;

    create_payload(id, p->get_receiver_id(), p->get_sender_id(), p->get_items(), p->get_travel_time(), m_time + m_time - (p->get_arrival_time() - p->get_travel_time()));
    process_arrived_payloads();
    return true;
}