# Copyright 2020-2023 Bloomberg Finance L.P.
# SPDX-License-Identifier: Apache-2.0
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

include(${CMAKE_CURRENT_LIST_DIR}/variables.cmake)

ntf_repository(
    NAME
        ntf-core
    VERSION
        2.2.4
    PATH
        ${CMAKE_CURRENT_LIST_DIR}
    URL
        "http://github.com/bloomberg/ntf-core"
    UFID
        ${NTF_BUILD_UFID}
)

ntf_repository_enable_warnings(
    ${NTF_BUILD_WITH_WARNINGS})

ntf_repository_enable_warnings_as_errors(
    ${NTF_BUILD_WITH_WARNINGS_AS_ERRORS})

ntf_repository_enable_documentation(
    ${NTF_BUILD_WITH_DOCUMENTATION})

ntf_repository_enable_documentation_internal(
    ${NTF_BUILD_WITH_DOCUMENTATION_INTERNAL})

ntf_repository_enable_coverage(
    ${NTF_BUILD_WITH_COVERAGE})

if (${NTF_BUILD_WITH_NTS})
    ntf_group(
        NAME
            nts
        PATH
            groups/nts
        DESCRIPTION
            "Blocking and non-blocking sockets for network programming")

    if (${NTF_BUILD_WITH_BAL})
        ntf_group_requires(NAME nts DEPENDENCY bal)
    endif()

    if (${NTF_BUILD_WITH_BDL})
        ntf_group_requires(NAME nts DEPENDENCY bdl)
    endif()

    if (${NTF_BUILD_WITH_BSL})
        ntf_group_requires(NAME nts DEPENDENCY bsl)
    endif()

    ntf_package(NAME ntsscm)

    ntf_component(NAME ntsscm_version)
    ntf_component(NAME ntsscm_versiontag)

    ntf_package_end(NAME ntsscm)

    ntf_package(
        NAME
            ntscfg
        REQUIRES
            ntsscm
    )

    configure_file(
        ${CMAKE_CURRENT_LIST_DIR}/cmake/templates/ntscfg_config.h
        ${CMAKE_BINARY_DIR}/ntscfg_config.h
        @ONLY)

    configure_file(
        ${CMAKE_CURRENT_LIST_DIR}/cmake/templates/ntscfg_config.cpp
        ${CMAKE_BINARY_DIR}/ntscfg_config.cpp
        @ONLY)

    ntf_component(PATH ${CMAKE_BINARY_DIR}/ntscfg_config.cpp)
    ntf_component(NAME ntscfg_limits)
    ntf_component(NAME ntscfg_platform)
    ntf_component(NAME ntscfg_test)

    ntf_package_end(NAME ntscfg)

    ntf_package(
        NAME
            ntsa
        REQUIRES
            ntsscm
            ntscfg
    )

    ntf_component(NAME ntsa_adapter)
    ntf_component(NAME ntsa_abstract)
    ntf_component(NAME ntsa_buffer)
    ntf_component(NAME ntsa_data)
    ntf_component(NAME ntsa_datatype)
    ntf_component(NAME ntsa_distinguishedname)
    ntf_component(NAME ntsa_domainname)
    ntf_component(NAME ntsa_endpoint)
    ntf_component(NAME ntsa_endpointoptions)
    ntf_component(NAME ntsa_endpointtype)
    ntf_component(NAME ntsa_error)
    ntf_component(NAME ntsa_ethernetaddress)
    ntf_component(NAME ntsa_ethernetheader)
    ntf_component(NAME ntsa_ethernetprotocol)
    ntf_component(NAME ntsa_event)
    ntf_component(NAME ntsa_file)
    ntf_component(NAME ntsa_guid)
    ntf_component(NAME ntsa_handle)
    ntf_component(NAME ntsa_id)
    ntf_component(NAME ntsa_interest)
    ntf_component(NAME ntsa_host)
    ntf_component(NAME ntsa_hosttype)
    ntf_component(NAME ntsa_ipendpoint)
    ntf_component(NAME ntsa_ipv4endpoint)
    ntf_component(NAME ntsa_ipv6endpoint)
    ntf_component(NAME ntsa_ipaddress)
    ntf_component(NAME ntsa_ipaddressoptions)
    ntf_component(NAME ntsa_ipaddresstype)
    ntf_component(NAME ntsa_ipv4address)
    ntf_component(NAME ntsa_ipv6address)
    ntf_component(NAME ntsa_ipv6scopeid)
    ntf_component(NAME ntsa_linger)
    ntf_component(NAME ntsa_localname)
    ntf_component(NAME ntsa_message)
    ntf_component(NAME ntsa_notification)
    ntf_component(NAME ntsa_notificationqueue)
    ntf_component(NAME ntsa_notificationtype)
    ntf_component(NAME ntsa_port)
    ntf_component(NAME ntsa_portoptions)
    ntf_component(NAME ntsa_reactorconfig)
    ntf_component(NAME ntsa_receivecontext)
    ntf_component(NAME ntsa_receiveoptions)
    ntf_component(NAME ntsa_resolverconfig)
    ntf_component(NAME ntsa_sendcontext)
    ntf_component(NAME ntsa_sendoptions)
    ntf_component(NAME ntsa_shutdowntype)
    ntf_component(NAME ntsa_shutdownmode)
    ntf_component(NAME ntsa_shutdownorigin)
    ntf_component(NAME ntsa_socketconfig)
    ntf_component(NAME ntsa_socketoption)
    ntf_component(NAME ntsa_socketoptiontype)
    ntf_component(NAME ntsa_socketinfo)
    ntf_component(NAME ntsa_socketinfofilter)
    ntf_component(NAME ntsa_socketstate)
    ntf_component(NAME ntsa_tcpcongestioncontrol)
    ntf_component(NAME ntsa_tcpcongestioncontrolalgorithm)
    ntf_component(NAME ntsa_temporary)
    ntf_component(NAME ntsa_transport)
    ntf_component(NAME ntsa_timestamp)
    ntf_component(NAME ntsa_timestamptype)
    ntf_component(NAME ntsa_uri)
    ntf_component(NAME ntsa_zerocopy)

    ntf_package_end(NAME ntsa)

    ntf_package(
        NAME
            ntsi
        REQUIRES
            ntsscm
            ntscfg
            ntsa
    )

    ntf_component(NAME ntsi_channel)
    ntf_component(NAME ntsi_datagramsocket)
    ntf_component(NAME ntsi_datagramsocketfactory)
    ntf_component(NAME ntsi_descriptor)
    ntf_component(NAME ntsi_listenersocket)
    ntf_component(NAME ntsi_listenersocketfactory)
    ntf_component(NAME ntsi_reactor)
    ntf_component(NAME ntsi_resolver)
    ntf_component(NAME ntsi_streamsocket)
    ntf_component(NAME ntsi_streamsocketfactory)

    ntf_package_end(NAME ntsi)

    ntf_package(
        NAME
            ntsd
        REQUIRES
            ntsscm
            ntscfg
            ntsa
            ntsi
        PRIVATE
    )

    ntf_component(NAME ntsd_datautil)
    ntf_component(NAME ntsd_message)
    ntf_component(NAME ntsd_messageheader)
    ntf_component(NAME ntsd_messageparser)
    ntf_component(NAME ntsd_messagetype)

    ntf_package_end(NAME ntsd)

    ntf_package(
        NAME
            ntsu
        REQUIRES
            ntsscm
            ntscfg
            ntsa
            ntsi
            ntsd
        PRIVATE
    )

    ntf_component(NAME ntsu_adapterutil)
    ntf_component(NAME ntsu_zerocopyutil)
    ntf_component(NAME ntsu_resolverutil)
    ntf_component(NAME ntsu_socketutil)
    ntf_component(NAME ntsu_socketoptionutil)
    ntf_component(NAME ntsu_timestamputil)

    ntf_package_end(NAME ntsu)

    ntf_package(
        NAME
            ntsb
        REQUIRES
            ntsscm
            ntscfg
            ntsa
            ntsi
            ntsd
            ntsu
        PRIVATE
    )

    ntf_component(NAME ntsb_controller)
    ntf_component(NAME ntsb_datagramsocket)
    ntf_component(NAME ntsb_listenersocket)
    ntf_component(NAME ntsb_resolver)
    ntf_component(NAME ntsb_streamsocket)

    ntf_package_end(NAME ntsb)

    ntf_package(
        NAME
            ntso
        REQUIRES
            ntsscm
            ntscfg
            ntsa
            ntsi
            ntsd
            ntsu
            ntsb
        PRIVATE
    )

    ntf_component(NAME ntso_devpoll)
    ntf_component(NAME ntso_epoll)
    ntf_component(NAME ntso_eventport)
    ntf_component(NAME ntso_kqueue)
    ntf_component(NAME ntso_poll)
    ntf_component(NAME ntso_pollset)
    ntf_component(NAME ntso_select)
    ntf_component(NAME ntso_test)

    ntf_package_end(NAME ntso)

    ntf_package(
        NAME
            ntsf
        REQUIRES
            ntsscm
            ntscfg
            ntsa
            ntsi
            ntsd
            ntsu
            ntsb
            ntso
    )

    ntf_component(NAME ntsf_system)

    ntf_package_end(NAME ntsf)

    ntf_group_end(NAME nts)
endif()

if (${NTF_BUILD_WITH_NTC})
    ntf_group(
        NAME
            ntc
        PATH
            groups/ntc
        REQUIRES
            nts
        DESCRIPTION
            "Asynchronous sockets, timers, event loops, and thread pools for network programming"
    )

    if (${NTF_BUILD_WITH_BAL})
        ntf_group_requires(NAME ntc DEPENDENCY bal)
    endif()

    if (${NTF_BUILD_WITH_BDL})
        ntf_group_requires(NAME ntc DEPENDENCY bdl)
    endif()

    if (${NTF_BUILD_WITH_BSL})
        ntf_group_requires(NAME ntc DEPENDENCY bsl)
    endif()

    ntf_package(NAME ntcscm)

    ntf_component(NAME ntcscm_version)
    ntf_component(NAME ntcscm_versiontag)

    ntf_package_end(NAME ntcscm)

    ntf_package(
        NAME
            ntccfg
        REQUIRES
            ntcscm
    )

    configure_file(
        ${CMAKE_CURRENT_LIST_DIR}/cmake/templates/ntccfg_config.h
        ${CMAKE_BINARY_DIR}/ntccfg_config.h
        @ONLY)

    configure_file(
        ${CMAKE_CURRENT_LIST_DIR}/cmake/templates/ntccfg_config.cpp
        ${CMAKE_BINARY_DIR}/ntccfg_config.cpp
        @ONLY)

    ntf_component(PATH ${CMAKE_BINARY_DIR}/ntccfg_config.cpp)

    ntf_component(NAME ntccfg_bind)
    ntf_component(NAME ntccfg_foreach)
    ntf_component(NAME ntccfg_function)
    ntf_component(NAME ntccfg_inline)
    ntf_component(NAME ntccfg_likely)
    ntf_component(NAME ntccfg_limits)
    ntf_component(NAME ntccfg_lock)
    ntf_component(NAME ntccfg_mutex)
    ntf_component(NAME ntccfg_object)
    ntf_component(NAME ntccfg_platform)
    ntf_component(NAME ntccfg_test)
    ntf_component(NAME ntccfg_traits)
    ntf_component(NAME ntccfg_tune)

    ntf_package_end(NAME ntccfg)

    ntf_package(
        NAME
            ntca
        REQUIRES
            ntcscm
            ntccfg
    )

    ntf_component(NAME ntca_accepttoken)
    ntf_component(NAME ntca_acceptoptions)
    ntf_component(NAME ntca_acceptcontext)
    ntf_component(NAME ntca_acceptevent)
    ntf_component(NAME ntca_accepteventtype)
    ntf_component(NAME ntca_acceptqueuecontext)
    ntf_component(NAME ntca_acceptqueueevent)
    ntf_component(NAME ntca_acceptqueueeventtype)
    ntf_component(NAME ntca_bindtoken)
    ntf_component(NAME ntca_bindoptions)
    ntf_component(NAME ntca_bindcontext)
    ntf_component(NAME ntca_bindevent)
    ntf_component(NAME ntca_bindeventtype)
    ntf_component(NAME ntca_connecttoken)
    ntf_component(NAME ntca_connectoptions)
    ntf_component(NAME ntca_connectcontext)
    ntf_component(NAME ntca_connectevent)
    ntf_component(NAME ntca_connecteventtype)
    ntf_component(NAME ntca_datagramsocketevent)
    ntf_component(NAME ntca_datagramsocketeventtype)
    ntf_component(NAME ntca_datagramsocketoptions)
    ntf_component(NAME ntca_downgradecontext)
    ntf_component(NAME ntca_downgradeevent)
    ntf_component(NAME ntca_downgradeeventtype)
    ntf_component(NAME ntca_driverconfig)
    ntf_component(NAME ntca_drivermechanism)
    ntf_component(NAME ntca_encryptionauthentication)
    ntf_component(NAME ntca_encryptionmethod)
    ntf_component(NAME ntca_encryptionrole)
    ntf_component(NAME ntca_encryptionoptions)
    ntf_component(NAME ntca_encryptioncertificate)
    ntf_component(NAME ntca_encryptioncertificateoptions)
    ntf_component(NAME ntca_encryptionclientoptions)
    ntf_component(NAME ntca_encryptionkey)
    ntf_component(NAME ntca_encryptionkeyoptions)
    ntf_component(NAME ntca_encryptionkeytype)
    ntf_component(NAME ntca_encryptionresource)
    ntf_component(NAME ntca_encryptionresourcetype)
    ntf_component(NAME ntca_encryptionresourcedescriptor)
    ntf_component(NAME ntca_encryptionresourceoptions)
    ntf_component(NAME ntca_encryptionsecret)
    ntf_component(NAME ntca_encryptionserveroptions)
    ntf_component(NAME ntca_encryptionvalidation)
    ntf_component(NAME ntca_errorcontext)
    ntf_component(NAME ntca_errorevent)
    ntf_component(NAME ntca_erroreventtype)
    ntf_component(NAME ntca_flowcontrolmode)
    ntf_component(NAME ntca_flowcontroltype)
    ntf_component(NAME ntca_getipaddresscontext)
    ntf_component(NAME ntca_getipaddressevent)
    ntf_component(NAME ntca_getipaddresseventtype)
    ntf_component(NAME ntca_getipaddressoptions)
    ntf_component(NAME ntca_getdomainnamecontext)
    ntf_component(NAME ntca_getdomainnameevent)
    ntf_component(NAME ntca_getdomainnameeventtype)
    ntf_component(NAME ntca_getdomainnameoptions)
    ntf_component(NAME ntca_getportcontext)
    ntf_component(NAME ntca_getportevent)
    ntf_component(NAME ntca_getporteventtype)
    ntf_component(NAME ntca_getportoptions)
    ntf_component(NAME ntca_getservicenamecontext)
    ntf_component(NAME ntca_getservicenameevent)
    ntf_component(NAME ntca_getservicenameeventtype)
    ntf_component(NAME ntca_getservicenameoptions)
    ntf_component(NAME ntca_getendpointcontext)
    ntf_component(NAME ntca_getendpointoptions)
    ntf_component(NAME ntca_getendpointevent)
    ntf_component(NAME ntca_getendpointeventtype)
    ntf_component(NAME ntca_listenersocketevent)
    ntf_component(NAME ntca_listenersocketeventtype)
    ntf_component(NAME ntca_listenersocketoptions)
    ntf_component(NAME ntca_loadbalancingoptions)
    ntf_component(NAME ntca_monitorableregistryconfig)
    ntf_component(NAME ntca_monitorablecollectorconfig)
    ntf_component(NAME ntca_interfaceconfig)
    ntf_component(NAME ntca_proactorconfig)
    ntf_component(NAME ntca_ratelimiterconfig)
    ntf_component(NAME ntca_reactorconfig)
    ntf_component(NAME ntca_reactorcontext)
    ntf_component(NAME ntca_reactorevent)
    ntf_component(NAME ntca_reactoreventoptions)
    ntf_component(NAME ntca_reactoreventtrigger)
    ntf_component(NAME ntca_reactoreventtype)
    ntf_component(NAME ntca_readqueuecontext)
    ntf_component(NAME ntca_readqueueevent)
    ntf_component(NAME ntca_readqueueeventtype)
    ntf_component(NAME ntca_receivetoken)
    ntf_component(NAME ntca_receiveoptions)
    ntf_component(NAME ntca_receivecontext)
    ntf_component(NAME ntca_receiveevent)
    ntf_component(NAME ntca_receiveeventtype)
    ntf_component(NAME ntca_resolverconfig)
    ntf_component(NAME ntca_resolversource)
    ntf_component(NAME ntca_schedulerconfig)
    ntf_component(NAME ntca_sendtoken)
    ntf_component(NAME ntca_sendoptions)
    ntf_component(NAME ntca_sendcontext)
    ntf_component(NAME ntca_sendevent)
    ntf_component(NAME ntca_sendeventtype)
    ntf_component(NAME ntca_shutdowncontext)
    ntf_component(NAME ntca_shutdownevent)
    ntf_component(NAME ntca_shutdowneventtype)
    ntf_component(NAME ntca_streamsocketevent)
    ntf_component(NAME ntca_streamsocketeventtype)
    ntf_component(NAME ntca_streamsocketoptions)
    ntf_component(NAME ntca_timercontext)
    ntf_component(NAME ntca_timerevent)
    ntf_component(NAME ntca_timereventtype)
    ntf_component(NAME ntca_timeroptions)
    ntf_component(NAME ntca_threadconfig)
    ntf_component(NAME ntca_upgradecontext)
    ntf_component(NAME ntca_upgradeevent)
    ntf_component(NAME ntca_upgradeeventtype)
    ntf_component(NAME ntca_upgradeoptions)
    ntf_component(NAME ntca_upgradetoken)
    ntf_component(NAME ntca_waiteroptions)
    ntf_component(NAME ntca_writequeuecontext)
    ntf_component(NAME ntca_writequeueevent)
    ntf_component(NAME ntca_writequeueeventtype)

    ntf_package_end(NAME ntca)

    ntf_package(
        NAME
            ntci
        REQUIRES
            ntcscm
            ntccfg
            ntca
    )

    ntf_component(NAME ntci_acceptcallback)
    ntf_component(NAME ntci_acceptcallbackfactory)
    ntf_component(NAME ntci_acceptfuture)
    ntf_component(NAME ntci_acceptresult)
    ntf_component(NAME ntci_acceptor)
    ntf_component(NAME ntci_authorization)
    ntf_component(NAME ntci_bindable)
    ntf_component(NAME ntci_bindcallback)
    ntf_component(NAME ntci_bindcallbackfactory)
    ntf_component(NAME ntci_bindfuture)
    ntf_component(NAME ntci_bindresult)
    ntf_component(NAME ntci_callback)
    ntf_component(NAME ntci_cancellation)
    ntf_component(NAME ntci_chronology)
    ntf_component(NAME ntci_clock)
    ntf_component(NAME ntci_connector)
    ntf_component(NAME ntci_connectcallback)
    ntf_component(NAME ntci_connectcallbackfactory)
    ntf_component(NAME ntci_connectfuture)
    ntf_component(NAME ntci_connectresult)
    ntf_component(NAME ntci_closable)
    ntf_component(NAME ntci_closecallback)
    ntf_component(NAME ntci_closecallbackfactory)
    ntf_component(NAME ntci_closefuture)
    ntf_component(NAME ntci_closeresult)
    ntf_component(NAME ntci_datapool)
    ntf_component(NAME ntci_datagramsocket)
    ntf_component(NAME ntci_datagramsocketfactory)
    ntf_component(NAME ntci_datagramsocketmanager)
    ntf_component(NAME ntci_datagramsocketsession)
    ntf_component(NAME ntci_driver)
    ntf_component(NAME ntci_driverfactory)
    ntf_component(NAME ntci_encryption)
    ntf_component(NAME ntci_encryptioncertificate)
    ntf_component(NAME ntci_encryptioncertificategenerator)
    ntf_component(NAME ntci_encryptioncertificatestorage)
    ntf_component(NAME ntci_encryptionclient)
    ntf_component(NAME ntci_encryptionclientfactory)
    ntf_component(NAME ntci_encryptiondriver)
    ntf_component(NAME ntci_encryptionkey)
    ntf_component(NAME ntci_encryptionkeygenerator)
    ntf_component(NAME ntci_encryptionkeystorage)
    ntf_component(NAME ntci_encryptionresource)
    ntf_component(NAME ntci_encryptionresourcefactory)
    ntf_component(NAME ntci_encryptionserver)
    ntf_component(NAME ntci_encryptionserverfactory)
    ntf_component(NAME ntci_executor)
    ntf_component(NAME ntci_getipaddresscallback)
    ntf_component(NAME ntci_getipaddresscallbackfactory)
    ntf_component(NAME ntci_getdomainnamecallback)
    ntf_component(NAME ntci_getdomainnamecallbackfactory)
    ntf_component(NAME ntci_getportcallback)
    ntf_component(NAME ntci_getportcallbackfactory)
    ntf_component(NAME ntci_getservicenamecallback)
    ntf_component(NAME ntci_getservicenamecallbackfactory)
    ntf_component(NAME ntci_getendpointcallback)
    ntf_component(NAME ntci_getendpointcallbackfactory)
    ntf_component(NAME ntci_identifiable)
    ntf_component(NAME ntci_invoker)
    ntf_component(NAME ntci_interface)
    ntf_component(NAME ntci_listenersocket)
    ntf_component(NAME ntci_listenersocketfactory)
    ntf_component(NAME ntci_listenersocketmanager)
    ntf_component(NAME ntci_listenersocketsession)
    ntf_component(NAME ntci_log)
    ntf_component(NAME ntci_metric)
    ntf_component(NAME ntci_monitorable)
    ntf_component(NAME ntci_mutex)
    ntf_component(NAME ntci_ratelimiter)
    ntf_component(NAME ntci_ratelimiterfactory)
    ntf_component(NAME ntci_reactor)
    ntf_component(NAME ntci_reactorfactory)
    ntf_component(NAME ntci_reactormetrics)
    ntf_component(NAME ntci_reactorpool)
    ntf_component(NAME ntci_reactorsocket)
    ntf_component(NAME ntci_receivecallback)
    ntf_component(NAME ntci_receivecallbackfactory)
    ntf_component(NAME ntci_receivefuture)
    ntf_component(NAME ntci_receiveresult)
    ntf_component(NAME ntci_receiver)
    ntf_component(NAME ntci_reservation)
    ntf_component(NAME ntci_resolver)
    ntf_component(NAME ntci_resolverfactory)
    ntf_component(NAME ntci_proactor)
    ntf_component(NAME ntci_proactorfactory)
    ntf_component(NAME ntci_proactormetrics)
    ntf_component(NAME ntci_proactorsocket)
    ntf_component(NAME ntci_proactorpool)
    ntf_component(NAME ntci_sender)
    ntf_component(NAME ntci_sendcallback)
    ntf_component(NAME ntci_sendcallbackfactory)
    ntf_component(NAME ntci_sendfuture)
    ntf_component(NAME ntci_sendresult)
    ntf_component(NAME ntci_strand)
    ntf_component(NAME ntci_strandfactory)
    ntf_component(NAME ntci_streamsocket)
    ntf_component(NAME ntci_streamsocketfactory)
    ntf_component(NAME ntci_streamsocketmanager)
    ntf_component(NAME ntci_streamsocketsession)
    ntf_component(NAME ntci_scheduler)
    ntf_component(NAME ntci_timer)
    ntf_component(NAME ntci_timercallback)
    ntf_component(NAME ntci_timercallbackfactory)
    ntf_component(NAME ntci_timerfactory)
    ntf_component(NAME ntci_timerfuture)
    ntf_component(NAME ntci_timerresult)
    ntf_component(NAME ntci_timersession)
    ntf_component(NAME ntci_thread)
    ntf_component(NAME ntci_threadfactory)
    ntf_component(NAME ntci_threadpool)
    ntf_component(NAME ntci_upgradable)
    ntf_component(NAME ntci_upgradecallback)
    ntf_component(NAME ntci_upgradecallbackfactory)
    ntf_component(NAME ntci_user)
    ntf_component(NAME ntci_waiter)

    ntf_package_end(NAME ntci)

    ntf_package(
        NAME
            ntcm
        REQUIRES
            ntcscm
            ntccfg
            ntca
            ntci
        PRIVATE
    )

    ntf_component(NAME ntcm_collector)
    ntf_component(NAME ntcm_logpublisher)
    ntf_component(NAME ntcm_monitorableregistry)
    ntf_component(NAME ntcm_monitorableutil)
    ntf_component(NAME ntcm_periodiccollector)

    ntf_package_end(NAME ntcm)

    ntf_package(
        NAME
            ntcs
        REQUIRES
            ntcscm
            ntccfg
            ntca
            ntci
            ntcm
        PRIVATE
    )

    ntf_component(NAME ntcs_async)
    ntf_component(NAME ntcs_authorization)
    ntf_component(NAME ntcs_blobbufferfactory)
    ntf_component(NAME ntcs_blobbufferutil)
    ntf_component(NAME ntcs_blobutil)
    ntf_component(NAME ntcs_callbackstate)
    ntf_component(NAME ntcs_chronology)
    ntf_component(NAME ntcs_compat)
    ntf_component(NAME ntcs_controller)
    ntf_component(NAME ntcs_datapool)
    ntf_component(NAME ntcs_detachstate)
    ntf_component(NAME ntcs_dispatch)
    ntf_component(NAME ntcs_driver)
    ntf_component(NAME ntcs_event)
    ntf_component(NAME ntcs_flowcontrolcontext)
    ntf_component(NAME ntcs_flowcontrolstate)
    ntf_component(NAME ntcs_global)
    ntf_component(NAME ntcs_globalallocator)
    ntf_component(NAME ntcs_globalexecutor)
    ntf_component(NAME ntcs_interest)
    ntf_component(NAME ntcs_leakybucket)
    ntf_component(NAME ntcs_memorymap)
    ntf_component(NAME ntcs_metrics)
    ntf_component(NAME ntcs_nomenclature)
    ntf_component(NAME ntcs_observer)
    ntf_component(NAME ntcs_openstate)
    ntf_component(NAME ntcs_plugin)
    ntf_component(NAME ntcs_proactordetachcontext)
    ntf_component(NAME ntcs_proactormetrics)
    ntf_component(NAME ntcs_processutil)
    ntf_component(NAME ntcs_processmetrics)
    ntf_component(NAME ntcs_processstatistics)
    ntf_component(NAME ntcs_ratelimiter)
    ntf_component(NAME ntcs_reactormetrics)
    ntf_component(NAME ntcs_registry)
    ntf_component(NAME ntcs_reservation)
    ntf_component(NAME ntcs_shutdowncontext)
    ntf_component(NAME ntcs_shutdownstate)
    ntf_component(NAME ntcs_skiplist)
    ntf_component(NAME ntcs_strand)
    ntf_component(NAME ntcs_threadutil)
    ntf_component(NAME ntcs_watermarks)
    ntf_component(NAME ntcs_watermarkutil)
    ntf_component(NAME ntcs_user)

    ntf_package_end(NAME ntcs)

    ntf_package(
        NAME
            ntcq
        REQUIRES
            ntcscm
            ntccfg
            ntca
            ntci
            ntcm
            ntcs
        PRIVATE
    )

    ntf_component(NAME ntcq_accept)
    ntf_component(NAME ntcq_bind)
    ntf_component(NAME ntcq_connect)
    ntf_component(NAME ntcq_receive)
    ntf_component(NAME ntcq_send)
    ntf_component(NAME ntcq_zerocopy)

    ntf_package_end(NAME ntcq)

    ntf_package(
        NAME
            ntcd
        REQUIRES
            ntcscm
            ntccfg
            ntca
            ntci
            ntcm
            ntcs
            ntcq
        PRIVATE
    )

    ntf_component(NAME ntcd_blobbufferfactory)
    ntf_component(NAME ntcd_datagramsocket)
    ntf_component(NAME ntcd_datapool)
    ntf_component(NAME ntcd_datautil)
    ntf_component(NAME ntcd_encryption)
    ntf_component(NAME ntcd_listenersocket)
    ntf_component(NAME ntcd_machine)
    ntf_component(NAME ntcd_proactor)
    ntf_component(NAME ntcd_reactor)
    ntf_component(NAME ntcd_resolver)
    ntf_component(NAME ntcd_streamsocket)
    ntf_component(NAME ntcd_simulation)
    ntf_component(NAME ntcd_timer)

    ntf_package_end(NAME ntcd)

    ntf_package(
        NAME
            ntcdns
        REQUIRES
            ntcscm
            ntccfg
            ntca
            ntci
            ntcm
            ntcs
            ntcq
            ntcd
        PRIVATE
    )

    ntf_component(NAME ntcdns_cache)
    ntf_component(NAME ntcdns_client)
    ntf_component(NAME ntcdns_compat)
    ntf_component(NAME ntcdns_database)
    ntf_component(NAME ntcdns_protocol)
    ntf_component(NAME ntcdns_resolver)
    ntf_component(NAME ntcdns_server)
    ntf_component(NAME ntcdns_system)
    ntf_component(NAME ntcdns_utility)
    ntf_component(NAME ntcdns_vocabulary)

    ntf_package_end(NAME ntcdns)

    ntf_package(
        NAME
            ntcu
        REQUIRES
            ntcscm
            ntccfg
            ntca
            ntci
            ntcm
            ntcs
            ntcq
            ntcd
            ntcdns
        PRIVATE
    )

    ntf_component(NAME ntcu_datagramsocketsession)
    ntf_component(NAME ntcu_datagramsocketeventqueue)
    ntf_component(NAME ntcu_datagramsocketutil)
    ntf_component(NAME ntcu_listenersocketsession)
    ntf_component(NAME ntcu_listenersocketeventqueue)
    ntf_component(NAME ntcu_listenersocketutil)
    ntf_component(NAME ntcu_streamsocketsession)
    ntf_component(NAME ntcu_streamsocketeventqueue)
    ntf_component(NAME ntcu_streamsocketutil)
    ntf_component(NAME ntcu_timestampcorrelator)

    ntf_package_end(NAME ntcu)

    ntf_package(
        NAME
            ntcr
        REQUIRES
            ntcscm
            ntccfg
            ntca
            ntci
            ntcm
            ntcs
            ntcq
            ntcd
            ntcdns
            ntcu
        PRIVATE
    )

    ntf_component(NAME ntcr_datagramsocket)
    ntf_component(NAME ntcr_listenersocket)
    ntf_component(NAME ntcr_streamsocket)
    ntf_component(NAME ntcr_interface)
    ntf_component(NAME ntcr_thread)

    ntf_package_end(NAME ntcr)

    ntf_package(
        NAME
            ntcp
        REQUIRES
            ntcscm
            ntccfg
            ntca
            ntci
            ntcm
            ntcs
            ntcq
            ntcd
            ntcdns
            ntcu
        PRIVATE
    )

    ntf_component(NAME ntcp_datagramsocket)
    ntf_component(NAME ntcp_listenersocket)
    ntf_component(NAME ntcp_streamsocket)
    ntf_component(NAME ntcp_interface)
    ntf_component(NAME ntcp_thread)

    ntf_package_end(NAME ntcp)

    ntf_package(
        NAME
            ntco
        REQUIRES
            ntcscm
            ntccfg
            ntca
            ntci
            ntcm
            ntcs
            ntcq
            ntcd
            ntcdns
            ntcu
            ntcr
            ntcp
        PRIVATE
    )

    ntf_component(NAME ntco_devpoll)
    ntf_component(NAME ntco_epoll)
    ntf_component(NAME ntco_eventport)
    ntf_component(NAME ntco_iocp)
    ntf_component(NAME ntco_ioring)
    ntf_component(NAME ntco_kqueue)
    ntf_component(NAME ntco_poll)
    ntf_component(NAME ntco_pollset)
    ntf_component(NAME ntco_select)

    ntf_package_end(NAME ntco)

    ntf_package(
        NAME
            ntcf
        REQUIRES
            ntcscm
            ntccfg
            ntca
            ntci
            ntcm
            ntcs
            ntcq
            ntcd
            ntcdns
            ntcu
            ntcr
            ntcp
            ntco
    )

    ntf_component(NAME ntcf_api)
    ntf_component(NAME ntcf_system)
    ntf_component(NAME ntcf_test)

    ntf_package_end(NAME ntcf)

    ntf_group_end(NAME ntc)
endif()

if (${NTF_BUILD_WITH_USAGE_EXAMPLES})
    if (${NTF_BUILD_WITH_NTS})
        foreach (suffix 01;02;03;04;05;06;07;08)
            ntf_executable(
                NAME
                    ntsu${suffix}
                PATH
                    examples/m_ntsu${suffix}
                REQUIRES
                    nts
                PRIVATE TEST EXAMPLE)

            ntf_executable_end(NAME ntsu${suffix})
        endforeach()
    endif()

    if (${NTF_BUILD_WITH_NTC})
        foreach (suffix 01;02;03;04;05;06;07;08;09;10;11;12;13;14)
            ntf_executable(
                NAME
                    ntcu${suffix}
                PATH
                    examples/m_ntcu${suffix}
                REQUIRES
                    ntc nts
                PRIVATE TEST EXAMPLE)

            ntf_executable_end(NAME ntcu${suffix})
        endforeach()
    endif()
endif()

if (VERBOSE)
    ntf_target_dump(bsl)
    ntf_target_dump(bdl)
    ntf_target_dump(bal)
    ntf_target_dump(nts)
    ntf_target_dump(ntc)
endif()

ntf_repository_end()
