#include "ntp.h"

#include <string.h>
#include "simplelink.h"
#include "py/mpstate.h"
#include "py/runtime.h"
#include "modnetwork.h"

STATIC const mp_obj_type_t ntp_socket_type;

uint32_t get_ntp_utc(int *err) {

    // 1B followed by 47 nulls
    static char msg[] = "\x1b\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";

    for (mp_uint_t i = 0; i < MP_STATE_PORT(mod_network_nic_list).len; i++) {
        mp_obj_t nic = MP_STATE_PORT(mod_network_nic_list).items[i];
        mod_network_nic_type_t *nic_type = (mod_network_nic_type_t*)mp_obj_get_type(nic);

        if (nic_type->gethostbyname != NULL) {
            uint8_t ip[MOD_NETWORK_IPV4ADDR_BUF_SIZE];
            int32_t result = nic_type->gethostbyname(nic, "pool.ntp.org", 12, ip, AF_INET);

            if (result != 0) {
                return NTP_ERR_LOOKUP;
            }

            // Create the socket
            mod_network_socket_obj_t *s = m_new_obj(mod_network_socket_obj_t);

            s->base.type = (mp_obj_t)&ntp_socket_type;
            s->nic = MP_OBJ_NULL;
            s->nic_type = NULL;
            s->u_param.domain = AF_INET;
            s->u_param.type = SOCK_DGRAM;
            s->u_param.proto = IPPROTO_UDP;
            s->u_param.fileno = -1;

            // Connect the socket to the NIC
            s->nic = mod_network_find_nic(ip);
            s->nic_type = (mod_network_nic_type_t*)mp_obj_get_type(s->nic);

            if (s->nic_type->socket(s, err) != 0) {
                s->nic_type->close(s);
                m_del_obj(mod_network_socket_obj_t, s);
                return NTP_ERR_SOCKET;
            }

            // Connect to the ntp server
            if (s->nic_type->sendto(s, (byte*)msg, 48, ip, 123, err) == -1) {
                s->nic_type->close(s);
                m_del_obj(mod_network_socket_obj_t, s);
                return NTP_ERR_SEND;
            }

            uint32_t data[12];
            mp_uint_t port;

            if (s->nic_type->settimeout(s, 2000, err) != 0) {
                s->nic_type->close(s);
                m_del_obj(mod_network_socket_obj_t, s);
                return NTP_ERR_SET_TIMEOUT;
            }

            if (s->nic_type->recvfrom(s, (byte*)data, sizeof(data), ip, &port, err) == -1) {
                s->nic_type->close(s);
                m_del_obj(mod_network_socket_obj_t, s);
                return NTP_ERR_RECV;
            }

            s->nic_type->close(s);
            m_del_obj(mod_network_socket_obj_t, s);

            data[10] = ((data[10] & 0xFF000000) >> 24) |
                       ((data[10] & 0xFF0000) >> 8) |
                       ((data[10] & 0xFF00) << 8) |
                       ((data[10] & 0xFF) << 24);

            return data[10] - 2208988800u;
        }
    }

    return NTP_ERR_NO_NIC;
}
