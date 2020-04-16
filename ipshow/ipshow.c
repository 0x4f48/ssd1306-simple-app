/*
 * link up/down monitoring:
 * https://stackoverflow.com/questions/579783/how-to-detect-ip-address-change-programmatically-in-linux
 *
 */

#include <asm/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#include "ssd1306_i2c.h"
#include "list.h"

#define HOST_DBG    0

struct monif_info
{
     char if_name[16];
     char if_ipaddr[16];
     struct list_head list;
};

static struct monif_info  ifsinfo;


int read_ip(char* ifname, char* ipaddr, size_t len)
{
    int s;
    struct ifreq ifr = {};

    s = socket(PF_INET, SOCK_DGRAM, 0);

    strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));

    if (ioctl(s, SIOCGIFADDR, &ifr) >= 0)
    {
        snprintf( ipaddr, len, "%s", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
        return 0;
    }
    return -1;
}

void update_lcd()
{
    char disp_buffer[256];
    struct monif_info* iter;

    disp_buffer[0] = '\0';
    list_for_each_entry(iter, &ifsinfo.list, list)
    {
        char tmp_buffer[128];
        //printf("%s: %s\n", iter->if_name, iter->if_ipaddr);
        snprintf(tmp_buffer, sizeof(tmp_buffer), "%s:%s\n", iter->if_name, iter->if_ipaddr );
        strcat(disp_buffer, tmp_buffer );
    }
#if HOST_DBG
    printf("%s", disp_buffer);
#else
    ssd1306_setTextSize(1);
    ssd1306_clearDisplay();
    ssd1306_drawString(disp_buffer);
    ssd1306_display();
#endif
}

static void append_ifinfo(struct monif_info* ptr,const char* ifname)
{
    struct monif_info* tmp;
    tmp = (struct monif_info*)malloc(sizeof(struct monif_info));
    if(!tmp)
    {
        perror("malloc");
        exit(1);
    }
    strncpy(tmp->if_name, ifname, sizeof(tmp->if_name) );
    tmp->if_ipaddr[0] = '\0';
    list_add_tail( &(tmp->list), &(ptr->list) );
}

int main (int argc, char** argv)
{
    struct sockaddr_nl addr;
    int sock, len;
    char buffer[4096];
    struct nlmsghdr *nlh;

    struct monif_info* iter;
    int ret, i;

    // checking input parameters
    if ( argc < 2 )
    {
        fprintf(stderr, "Usage: ipshow [i2c device node] [network interface] ...\n");
         exit(1);
    }

    INIT_LIST_HEAD(&ifsinfo.list);

    for ( i = 2; i < argc; i++ )
    {
        append_ifinfo( &ifsinfo, argv[i] );
    }

#if !HOST_DBG
    // init i2c LCD
    ssd1306_begin(SSD1306_SWITCHCAPVCC, argv[1], SSD1306_I2C_ADDRESS);
#endif

    /////////////////////////////////////////////////////////
    // read initial ip
    list_for_each_entry(iter, &ifsinfo.list, list)
    {
        ret = read_ip(iter->if_name, iter->if_ipaddr, sizeof(iter->if_ipaddr));
        if ( ret != 0 )
        {
            snprintf(iter->if_ipaddr, sizeof(iter->if_ipaddr), "%s", "down");
        }
    }
    update_lcd();

    ///////////////////////////////////////////////////////////////
    // prepare monitoring
    if ((sock = socket(PF_NETLINK, SOCK_RAW, NETLINK_ROUTE)) == -1)
    {
        perror("couldn't open NETLINK_ROUTE socket");
        exit(1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.nl_family = AF_NETLINK;
    addr.nl_groups = RTMGRP_IPV4_IFADDR;

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        perror("couldn't bind");
        exit(1);
    }

    nlh = (struct nlmsghdr *)buffer;
    while ((len = recv(sock, nlh, 4096, 0)) > 0)
    {
        while ((NLMSG_OK(nlh, len)) && (nlh->nlmsg_type != NLMSG_DONE))
        {
            if (nlh->nlmsg_type == RTM_NEWADDR)
            {
                struct ifaddrmsg *ifa = (struct ifaddrmsg *) NLMSG_DATA(nlh);
                struct rtattr *rth = IFA_RTA(ifa);
                int rtl = IFA_PAYLOAD(nlh);

                while (rtl && RTA_OK(rth, rtl))
                {
                    if (rth->rta_type == IFA_LOCAL)
                    {
                        char name[IFNAMSIZ];
                        if_indextoname(ifa->ifa_index, name);
                        char ip[INET_ADDRSTRLEN];
                        inet_ntop(AF_INET, RTA_DATA(rth), ip, sizeof(ip));
                        // is it interface we are monitoring?
                        list_for_each_entry(iter, &ifsinfo.list, list)
                        {
                            if (strcmp(iter->if_name, name) == 0 )
                            {
                                //printf("interface %s ip: %s\n", name, ip);
                                strncpy(iter->if_ipaddr, ip, sizeof(iter->if_ipaddr) );
                            }
                        }
                        update_lcd();

                    }
                    rth = RTA_NEXT(rth, rtl);
                }
            }
            else if (nlh->nlmsg_type == RTM_DELADDR)
            {
                struct ifaddrmsg *ifa = (struct ifaddrmsg *) NLMSG_DATA(nlh);
                struct rtattr *rth = IFA_RTA(ifa);
                int rtl = IFA_PAYLOAD(nlh);

                while (rtl && RTA_OK(rth, rtl))
                {
                    if (rth->rta_type == IFA_LOCAL)
                    {
                        char name[IFNAMSIZ];
                        if_indextoname(ifa->ifa_index, name);
                        //printf("del interface %s\n", name);
                        list_for_each_entry(iter, &ifsinfo.list, list)
                        {
                            if (strcmp(iter->if_name, name) == 0 )
                            {
                                snprintf(iter->if_ipaddr, sizeof(iter->if_ipaddr), "%s", "down");
                            }
                        }
                        update_lcd();
                    }
                    rth = RTA_NEXT(rth, rtl);
                }
            }
            nlh = NLMSG_NEXT(nlh, len);
        }
    }

    return 0;
}


