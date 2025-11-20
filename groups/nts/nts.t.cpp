#include <ntscfg_config.h>

#if NTS_BUILD_UNITY == 0
#error Not expected
#endif

// ntsscm

// #include <ntsscm_version.t.cpp>
// #include <ntsscm_versiontag.t.cpp>

// ntscfg

#include <ntscfg_limits.t.cpp>
#include <ntscfg_mock.t.cpp>
#include <ntscfg_platform.t.cpp>
#include <ntscfg_test.t.cpp>

// ntsa

#include <ntsa_abstract.t.cpp>
#include <ntsa_adapter.t.cpp>
#include <ntsa_allocator.t.cpp>
#include <ntsa_backoff.t.cpp>
#include <ntsa_buffer.t.cpp>
#include <ntsa_coroutine.t.cpp>
#include <ntsa_data.t.cpp>
#include <ntsa_datatype.t.cpp>
#include <ntsa_distinguishedname.t.cpp>
#include <ntsa_domainname.t.cpp>
#include <ntsa_endpoint.t.cpp>
#include <ntsa_endpointoptions.t.cpp>
#include <ntsa_endpointtype.t.cpp>
#include <ntsa_error.t.cpp>
#include <ntsa_ethernetaddress.t.cpp>
#include <ntsa_ethernetheader.t.cpp>
#include <ntsa_ethernetprotocol.t.cpp>
#include <ntsa_event.t.cpp>
#include <ntsa_file.t.cpp>
#include <ntsa_guid.t.cpp>
#include <ntsa_handle.t.cpp>
#include <ntsa_host.t.cpp>
#include <ntsa_hosttype.t.cpp>
#include <ntsa_id.t.cpp>
#include <ntsa_interest.t.cpp>
#include <ntsa_ipaddress.t.cpp>
#include <ntsa_ipaddressoptions.t.cpp>
#include <ntsa_ipaddresstype.t.cpp>
#include <ntsa_ipendpoint.t.cpp>
#include <ntsa_ipv4address.t.cpp>
#include <ntsa_ipv4endpoint.t.cpp>
#include <ntsa_ipv6address.t.cpp>
#include <ntsa_ipv6endpoint.t.cpp>
#include <ntsa_ipv6scopeid.t.cpp>
#include <ntsa_linger.t.cpp>
#include <ntsa_localname.t.cpp>
#include <ntsa_message.t.cpp>
#include <ntsa_notification.t.cpp>
#include <ntsa_notificationqueue.t.cpp>
#include <ntsa_notificationtype.t.cpp>
#include <ntsa_port.t.cpp>
#include <ntsa_portoptions.t.cpp>
#include <ntsa_reactorconfig.t.cpp>
#include <ntsa_receivecontext.t.cpp>
#include <ntsa_receiveoptions.t.cpp>
#include <ntsa_resolverconfig.t.cpp>
#include <ntsa_sendcontext.t.cpp>
#include <ntsa_sendoptions.t.cpp>
#include <ntsa_shutdownmode.t.cpp>
#include <ntsa_shutdownorigin.t.cpp>
#include <ntsa_shutdowntype.t.cpp>
#include <ntsa_socketconfig.t.cpp>
#include <ntsa_socketinfo.t.cpp>
#include <ntsa_socketinfofilter.t.cpp>
#include <ntsa_socketoption.t.cpp>
#include <ntsa_socketoptiontype.t.cpp>
#include <ntsa_socketstate.t.cpp>
#include <ntsa_tcpcongestioncontrol.t.cpp>
#include <ntsa_tcpcongestioncontrolalgorithm.t.cpp>
#include <ntsa_temporary.t.cpp>
#include <ntsa_timestamp.t.cpp>
#include <ntsa_timestamptype.t.cpp>
#include <ntsa_transport.t.cpp>
#include <ntsa_uri.t.cpp>
#include <ntsa_zerocopy.t.cpp>

// ntsi

// #include <ntsi_channel.t.cpp>
// #include <ntsi_datagramsocket.t.cpp>
// #include <ntsi_datagramsocketfactory.t.cpp>
// #include <ntsi_descriptor.t.cpp>
// #include <ntsi_listenersocket.t.cpp>
// #include <ntsi_listenersocketfactory.t.cpp>
// #include <ntsi_reactor.t.cpp>
// #include <ntsi_resolver.t.cpp>
// #include <ntsi_streamsocket.t.cpp>
// #include <ntsi_streamsocketfactory.t.cpp>

// ntsd

#include <ntsd_datautil.t.cpp>
#include <ntsd_message.t.cpp>
#include <ntsd_messageheader.t.cpp>
#include <ntsd_messageparser.t.cpp>
#include <ntsd_messagetype.t.cpp>

// ntsu

#include <ntsu_adapterutil.t.cpp>
#include <ntsu_resolverutil.t.cpp>
#include <ntsu_socketoptionutil.t.cpp>
#include <ntsu_socketutil.t.cpp>
#include <ntsu_timestamputil.t.cpp>
#include <ntsu_zerocopyutil.t.cpp>

// ntsb

#include <ntsb_controller.t.cpp>
#include <ntsb_datagramsocket.t.cpp>
#include <ntsb_listenersocket.t.cpp>
#include <ntsb_resolver.t.cpp>
#include <ntsb_streamsocket.t.cpp>

// ntso

#include <ntso_devpoll.t.cpp>
#include <ntso_epoll.t.cpp>
#include <ntso_eventport.t.cpp>
#include <ntso_kqueue.t.cpp>
#include <ntso_poll.t.cpp>
#include <ntso_pollset.t.cpp>
#include <ntso_select.t.cpp>
#include <ntso_test.t.cpp>

// ntsf

#include <ntsf_system.t.cpp>

