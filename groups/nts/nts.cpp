#include <ntscfg_config.h>

#if NTS_BUILD_UNITY == 0
#error Not expected
#endif

// ntsscm

#include <ntsscm_version.cpp>
#include <ntsscm_versiontag.cpp>

// ntscfg

#include <ntscfg_limits.cpp>
#include <ntscfg_mock.cpp>
#include <ntscfg_platform.cpp>
#include <ntscfg_test.cpp>

// ntsa

#include <ntsa_abstract.cpp>
#include <ntsa_adapter.cpp>
#include <ntsa_allocator.cpp>
#include <ntsa_buffer.cpp>
#include <ntsa_coroutine.cpp>
#include <ntsa_data.cpp>
#include <ntsa_datatype.cpp>
#include <ntsa_distinguishedname.cpp>
#include <ntsa_domainname.cpp>
#include <ntsa_endpoint.cpp>
#include <ntsa_endpointoptions.cpp>
#include <ntsa_endpointtype.cpp>
#include <ntsa_error.cpp>
#include <ntsa_ethernetaddress.cpp>
#include <ntsa_ethernetheader.cpp>
#include <ntsa_ethernetprotocol.cpp>
#include <ntsa_event.cpp>
#include <ntsa_file.cpp>
#include <ntsa_guid.cpp>
#include <ntsa_handle.cpp>
#include <ntsa_host.cpp>
#include <ntsa_hosttype.cpp>
#include <ntsa_id.cpp>
#include <ntsa_interest.cpp>
#include <ntsa_ipaddress.cpp>
#include <ntsa_ipaddressoptions.cpp>
#include <ntsa_ipaddresstype.cpp>
#include <ntsa_ipendpoint.cpp>
#include <ntsa_ipv4address.cpp>
#include <ntsa_ipv4endpoint.cpp>
#include <ntsa_ipv6address.cpp>
#include <ntsa_ipv6endpoint.cpp>
#include <ntsa_ipv6scopeid.cpp>
#include <ntsa_linger.cpp>
#include <ntsa_localname.cpp>
#include <ntsa_message.cpp>
#include <ntsa_notification.cpp>
#include <ntsa_notificationqueue.cpp>
#include <ntsa_notificationtype.cpp>
#include <ntsa_port.cpp>
#include <ntsa_portoptions.cpp>
#include <ntsa_reactorconfig.cpp>
#include <ntsa_receivecontext.cpp>
#include <ntsa_receiveoptions.cpp>
#include <ntsa_resolverconfig.cpp>
#include <ntsa_sendcontext.cpp>
#include <ntsa_sendoptions.cpp>
#include <ntsa_shutdownmode.cpp>
#include <ntsa_shutdownorigin.cpp>
#include <ntsa_shutdowntype.cpp>
#include <ntsa_socketconfig.cpp>
#include <ntsa_socketinfo.cpp>
#include <ntsa_socketinfofilter.cpp>
#include <ntsa_socketoption.cpp>
#include <ntsa_socketoptiontype.cpp>
#include <ntsa_socketstate.cpp>
#include <ntsa_tcpcongestioncontrol.cpp>
#include <ntsa_tcpcongestioncontrolalgorithm.cpp>
#include <ntsa_temporary.cpp>
#include <ntsa_timestamp.cpp>
#include <ntsa_timestamptype.cpp>
#include <ntsa_transport.cpp>
#include <ntsa_uri.cpp>
#include <ntsa_zerocopy.cpp>

// ntsi

#include <ntsi_channel.cpp>
#include <ntsi_datagramsocket.cpp>
#include <ntsi_datagramsocketfactory.cpp>
#include <ntsi_descriptor.cpp>
#include <ntsi_listenersocket.cpp>
#include <ntsi_listenersocketfactory.cpp>
#include <ntsi_reactor.cpp>
#include <ntsi_resolver.cpp>
#include <ntsi_streamsocket.cpp>
#include <ntsi_streamsocketfactory.cpp>

// ntsd

#include <ntsd_datautil.cpp>
#include <ntsd_message.cpp>
#include <ntsd_messageheader.cpp>
#include <ntsd_messageparser.cpp>
#include <ntsd_messagetype.cpp>

// ntsu

#include <ntsu_adapterutil.cpp>
#include <ntsu_resolverutil.cpp>
#include <ntsu_socketoptionutil.cpp>
#include <ntsu_socketutil.cpp>
#include <ntsu_timestamputil.cpp>
#include <ntsu_zerocopyutil.cpp>

// ntsb

#include <ntsb_controller.cpp>
#include <ntsb_datagramsocket.cpp>
#include <ntsb_listenersocket.cpp>
#include <ntsb_resolver.cpp>
#include <ntsb_streamsocket.cpp>

// ntso

#include <ntso_devpoll.cpp>
#include <ntso_epoll.cpp>
#include <ntso_eventport.cpp>
#include <ntso_kqueue.cpp>
#include <ntso_poll.cpp>
#include <ntso_pollset.cpp>
#include <ntso_select.cpp>
#include <ntso_test.cpp>

// ntsf

#include <ntsf_system.cpp>

