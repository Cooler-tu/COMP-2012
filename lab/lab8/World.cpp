#include "World.h"
#include "Base.h"
#include "Payload.h"
#include <iostream>
using std::cout;

World::~World() {
    while(!m_payload_queue.empty())
    {
        Payload* x = m_payload_queue.top();
        m_payload_queue.pop();
        delete x;
    }

    for(const pair<const string, Base *> &entry : m_bases)
        delete entry.second;
}

bool World::remove_payload(const string &id) {
    map<string, Payload *>::iterator r = m_payloads.find(id);

    if(r == m_payloads.end())
        return false;
    
    r->second->lazy_delete();
    m_payloads.erase(r);
    return true;
}

void World::elapse(int time) {
    m_time += time;
    process_arrived_payloads();
}

void World::process_arrived_payloads() {
    while (!m_payload_queue.empty()) 
    {
        Payload *x = m_payload_queue.top();
        if (x->get_arrival_time() > m_time)
            break;

        m_payload_queue.pop();

        if (!x->get_deleted()) {
            Base *receiver = m_bases.find(x->get_receiver_id())->second;
            *receiver += *x;
            cout << "Payload " << x->get_id() << " has arrived.\n";
            m_payloads.erase(x->get_id());
        }

        delete x;
    }
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