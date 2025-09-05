// Copyright 2020-2023 Bloomberg Finance L.P.
// SPDX-License-Identifier: Apache-2.0
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_NTS
#define INCLUDED_NTS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsscm_version.h>
#include <ntsscm_versiontag.h>

#include <ntscfg_config.h>
#include <ntscfg_limits.h>
#include <ntscfg_mock.h>
#include <ntscfg_platform.h>
#include <ntscfg_test.h>

#include <ntsa_abstract.h>
#include <ntsa_adapter.h>
#include <ntsa_allocator.h>
#include <ntsa_buffer.h>
#include <ntsa_coroutine.h>
#include <ntsa_data.h>
#include <ntsa_datatype.h>
#include <ntsa_distinguishedname.h>
#include <ntsa_domainname.h>
#include <ntsa_endpoint.h>
#include <ntsa_endpointoptions.h>
#include <ntsa_endpointtype.h>
#include <ntsa_error.h>
#include <ntsa_ethernetaddress.h>
#include <ntsa_ethernetheader.h>
#include <ntsa_ethernetprotocol.h>
#include <ntsa_event.h>
#include <ntsa_file.h>
#include <ntsa_guid.h>
#include <ntsa_handle.h>
#include <ntsa_host.h>
#include <ntsa_hosttype.h>
#include <ntsa_id.h>
#include <ntsa_interest.h>
#include <ntsa_ipaddress.h>
#include <ntsa_ipaddressoptions.h>
#include <ntsa_ipaddresstype.h>
#include <ntsa_ipendpoint.h>
#include <ntsa_ipv4address.h>
#include <ntsa_ipv4endpoint.h>
#include <ntsa_ipv6address.h>
#include <ntsa_ipv6endpoint.h>
#include <ntsa_ipv6scopeid.h>
#include <ntsa_linger.h>
#include <ntsa_localname.h>
#include <ntsa_message.h>
#include <ntsa_notification.h>
#include <ntsa_notificationqueue.h>
#include <ntsa_notificationtype.h>
#include <ntsa_port.h>
#include <ntsa_portoptions.h>
#include <ntsa_reactorconfig.h>
#include <ntsa_receivecontext.h>
#include <ntsa_receiveoptions.h>
#include <ntsa_resolverconfig.h>
#include <ntsa_sendcontext.h>
#include <ntsa_sendoptions.h>
#include <ntsa_shutdownmode.h>
#include <ntsa_shutdownorigin.h>
#include <ntsa_shutdowntype.h>
#include <ntsa_socketconfig.h>
#include <ntsa_socketinfo.h>
#include <ntsa_socketinfofilter.h>
#include <ntsa_socketoption.h>
#include <ntsa_socketoptiontype.h>
#include <ntsa_socketstate.h>
#include <ntsa_tcpcongestioncontrol.h>
#include <ntsa_tcpcongestioncontrolalgorithm.h>
#include <ntsa_temporary.h>
#include <ntsa_timestamp.h>
#include <ntsa_timestamptype.h>
#include <ntsa_transport.h>
#include <ntsa_uri.h>
#include <ntsa_zerocopy.h>

#include <ntsi_channel.h>
#include <ntsi_datagramsocket.h>
#include <ntsi_datagramsocketfactory.h>
#include <ntsi_descriptor.h>
#include <ntsi_listenersocket.h>
#include <ntsi_listenersocketfactory.h>
#include <ntsi_reactor.h>
#include <ntsi_resolver.h>
#include <ntsi_streamsocket.h>
#include <ntsi_streamsocketfactory.h>

#include <ntsf_system.h>

#endif
