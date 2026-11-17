template <class Container>
bool World::create_payload(const string &id, const string &sender_id, const string &receiver_id, const Container&items, int travel_time, int time_until_arrival) {
    if(m_payloads.find(id) != m_payloads.end() || 
        m_bases.find(sender_id) == m_bases.end() ||
        m_bases.find(receiver_id) == m_bases.end())
        return false;
    
    Base *sender = m_bases.find(sender_id)->second;
    if (!sender->can_afford(items)) return false;
    
    Payload *p = new Payload(id, sender_id, receiver_id, items, travel_time, time_until_arrival);
    m_payload_queue.push(p);
    m_payloads[id] = p;
    *sender -= *p;
    return true;
}