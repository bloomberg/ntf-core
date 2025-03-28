#include <ntccfg_config.h>

#if NTC_BUILD_UNITY == 0
#error Not expected
#endif

// ntcscm

#include <ntcscm_version.cpp>
#include <ntcscm_versiontag.cpp>

// ntccfg

#include <ntccfg_bind.cpp>
#include <ntccfg_bit.cpp>
#include <ntccfg_foreach.cpp>
#include <ntccfg_format.cpp>
#include <ntccfg_function.cpp>
#include <ntccfg_inline.cpp>
#include <ntccfg_likely.cpp>
#include <ntccfg_limits.cpp>
#include <ntccfg_lock.cpp>
#include <ntccfg_mutex.cpp>
#include <ntccfg_object.cpp>
#include <ntccfg_platform.cpp>
#include <ntccfg_traits.cpp>
#include <ntccfg_tune.cpp>

// ntca

#include <ntca_acceptcontext.cpp>
#include <ntca_acceptevent.cpp>
#include <ntca_accepteventtype.cpp>
#include <ntca_acceptoptions.cpp>
#include <ntca_acceptqueuecontext.cpp>
#include <ntca_acceptqueueevent.cpp>
#include <ntca_acceptqueueeventtype.cpp>
#include <ntca_accepttoken.cpp>
#include <ntca_bindcontext.cpp>
#include <ntca_bindevent.cpp>
#include <ntca_bindeventtype.cpp>
#include <ntca_bindoptions.cpp>
#include <ntca_bindtoken.cpp>
#include <ntca_checksum.cpp>
#include <ntca_checksumtype.cpp>
#include <ntca_compressionconfig.cpp>
#include <ntca_compressiongoal.cpp>
#include <ntca_compressiontype.cpp>
#include <ntca_connectcontext.cpp>
#include <ntca_connectevent.cpp>
#include <ntca_connecteventtype.cpp>
#include <ntca_connectoptions.cpp>
#include <ntca_connecttoken.cpp>
#include <ntca_datagramsocketevent.cpp>
#include <ntca_datagramsocketeventtype.cpp>
#include <ntca_datagramsocketoptions.cpp>
#include <ntca_deflatecontext.cpp>
#include <ntca_deflateoptions.cpp>
#include <ntca_downgradecontext.cpp>
#include <ntca_downgradeevent.cpp>
#include <ntca_downgradeeventtype.cpp>
#include <ntca_driverconfig.cpp>
#include <ntca_drivermechanism.cpp>
#include <ntca_encryptionauthentication.cpp>
#include <ntca_encryptioncertificate.cpp>
#include <ntca_encryptioncertificateoptions.cpp>
#include <ntca_encryptionclientoptions.cpp>
#include <ntca_encryptionkey.cpp>
#include <ntca_encryptionkeyoptions.cpp>
#include <ntca_encryptionkeytype.cpp>
#include <ntca_encryptionmethod.cpp>
#include <ntca_encryptionoptions.cpp>
#include <ntca_encryptionresource.cpp>
#include <ntca_encryptionresourcedescriptor.cpp>
#include <ntca_encryptionresourceoptions.cpp>
#include <ntca_encryptionresourcetype.cpp>
#include <ntca_encryptionrole.cpp>
#include <ntca_encryptionsecret.cpp>
#include <ntca_encryptionserveroptions.cpp>
#include <ntca_encryptionvalidation.cpp>
#include <ntca_errorcontext.cpp>
#include <ntca_errorevent.cpp>
#include <ntca_erroreventtype.cpp>
#include <ntca_flowcontrolmode.cpp>
#include <ntca_flowcontroltype.cpp>
#include <ntca_getdomainnamecontext.cpp>
#include <ntca_getdomainnameevent.cpp>
#include <ntca_getdomainnameeventtype.cpp>
#include <ntca_getdomainnameoptions.cpp>
#include <ntca_getendpointcontext.cpp>
#include <ntca_getendpointevent.cpp>
#include <ntca_getendpointeventtype.cpp>
#include <ntca_getendpointoptions.cpp>
#include <ntca_getipaddresscontext.cpp>
#include <ntca_getipaddressevent.cpp>
#include <ntca_getipaddresseventtype.cpp>
#include <ntca_getipaddressoptions.cpp>
#include <ntca_getportcontext.cpp>
#include <ntca_getportevent.cpp>
#include <ntca_getporteventtype.cpp>
#include <ntca_getportoptions.cpp>
#include <ntca_getservicenamecontext.cpp>
#include <ntca_getservicenameevent.cpp>
#include <ntca_getservicenameeventtype.cpp>
#include <ntca_getservicenameoptions.cpp>
#include <ntca_interfaceconfig.cpp>
#include <ntca_inflatecontext.cpp>
#include <ntca_inflateoptions.cpp>
#include <ntca_listenersocketevent.cpp>
#include <ntca_listenersocketeventtype.cpp>
#include <ntca_listenersocketoptions.cpp>
#include <ntca_loadbalancingoptions.cpp>
#include <ntca_monitorablecollectorconfig.cpp>
#include <ntca_monitorableregistryconfig.cpp>
#include <ntca_proactorconfig.cpp>
#include <ntca_ratelimiterconfig.cpp>
#include <ntca_reactorconfig.cpp>
#include <ntca_reactorcontext.cpp>
#include <ntca_reactorevent.cpp>
#include <ntca_reactoreventoptions.cpp>
#include <ntca_reactoreventtrigger.cpp>
#include <ntca_reactoreventtype.cpp>
#include <ntca_readqueuecontext.cpp>
#include <ntca_readqueueevent.cpp>
#include <ntca_readqueueeventtype.cpp>
#include <ntca_receivecontext.cpp>
#include <ntca_receiveevent.cpp>
#include <ntca_receiveeventtype.cpp>
#include <ntca_receiveoptions.cpp>
#include <ntca_receivetoken.cpp>
#include <ntca_resolverconfig.cpp>
#include <ntca_resolversource.cpp>
#include <ntca_schedulerconfig.cpp>
#include <ntca_sendcontext.cpp>
#include <ntca_sendevent.cpp>
#include <ntca_sendeventtype.cpp>
#include <ntca_sendoptions.cpp>
#include <ntca_sendtoken.cpp>
#include <ntca_serializationconfig.cpp>
#include <ntca_serializationtype.cpp>
#include <ntca_shutdowncontext.cpp>
#include <ntca_shutdownevent.cpp>
#include <ntca_shutdowneventtype.cpp>
#include <ntca_streamsocketevent.cpp>
#include <ntca_streamsocketeventtype.cpp>
#include <ntca_streamsocketoptions.cpp>
#include <ntca_threadconfig.cpp>
#include <ntca_timercontext.cpp>
#include <ntca_timerevent.cpp>
#include <ntca_timereventtype.cpp>
#include <ntca_timeroptions.cpp>
#include <ntca_upgradecontext.cpp>
#include <ntca_upgradeevent.cpp>
#include <ntca_upgradeeventtype.cpp>
#include <ntca_upgradeoptions.cpp>
#include <ntca_upgradetoken.cpp>
#include <ntca_waiteroptions.cpp>
#include <ntca_writequeuecontext.cpp>
#include <ntca_writequeueevent.cpp>
#include <ntca_writequeueeventtype.cpp>

// ntci

#include <ntci_acceptcallback.cpp>
#include <ntci_acceptcallbackfactory.cpp>
#include <ntci_acceptfuture.cpp>
#include <ntci_acceptor.cpp>
#include <ntci_acceptresult.cpp>
#include <ntci_authorization.cpp>
#include <ntci_bindable.cpp>
#include <ntci_bindcallback.cpp>
#include <ntci_bindcallbackfactory.cpp>
#include <ntci_bindfuture.cpp>
#include <ntci_bindresult.cpp>
#include <ntci_callback.cpp>
#include <ntci_cancellation.cpp>
#include <ntci_chronology.cpp>
#include <ntci_clock.cpp>
#include <ntci_closable.cpp>
#include <ntci_closecallback.cpp>
#include <ntci_closecallbackfactory.cpp>
#include <ntci_closefuture.cpp>
#include <ntci_closeresult.cpp>
#include <ntci_compression.cpp>
#include <ntci_compressiondriver.cpp>
#include <ntci_connectcallback.cpp>
#include <ntci_connectcallbackfactory.cpp>
#include <ntci_connectfuture.cpp>
#include <ntci_connector.cpp>
#include <ntci_connectresult.cpp>
#include <ntci_datagramsocket.cpp>
#include <ntci_datagramsocketfactory.cpp>
#include <ntci_datagramsocketmanager.cpp>
#include <ntci_datagramsocketsession.cpp>
#include <ntci_datapool.cpp>
#include <ntci_driver.cpp>
#include <ntci_driverfactory.cpp>
#include <ntci_encryption.cpp>
#include <ntci_encryptioncertificate.cpp>
#include <ntci_encryptioncertificategenerator.cpp>
#include <ntci_encryptioncertificatestorage.cpp>
#include <ntci_encryptionclient.cpp>
#include <ntci_encryptionclientfactory.cpp>
#include <ntci_encryptiondriver.cpp>
#include <ntci_encryptionkey.cpp>
#include <ntci_encryptionkeygenerator.cpp>
#include <ntci_encryptionkeystorage.cpp>
#include <ntci_encryptionresource.cpp>
#include <ntci_encryptionresourcefactory.cpp>
#include <ntci_encryptionserver.cpp>
#include <ntci_encryptionserverfactory.cpp>
#include <ntci_executor.cpp>
#include <ntci_getdomainnamecallback.cpp>
#include <ntci_getdomainnamecallbackfactory.cpp>
#include <ntci_getendpointcallback.cpp>
#include <ntci_getendpointcallbackfactory.cpp>
#include <ntci_getipaddresscallback.cpp>
#include <ntci_getipaddresscallbackfactory.cpp>
#include <ntci_getportcallback.cpp>
#include <ntci_getportcallbackfactory.cpp>
#include <ntci_getservicenamecallback.cpp>
#include <ntci_getservicenamecallbackfactory.cpp>
#include <ntci_identifiable.cpp>
#include <ntci_interface.cpp>
#include <ntci_invoker.cpp>
#include <ntci_listenersocket.cpp>
#include <ntci_listenersocketfactory.cpp>
#include <ntci_listenersocketmanager.cpp>
#include <ntci_listenersocketsession.cpp>
#include <ntci_log.cpp>
#include <ntci_metric.cpp>
#include <ntci_monitorable.cpp>
#include <ntci_mutex.cpp>
#include <ntci_proactor.cpp>
#include <ntci_proactorfactory.cpp>
#include <ntci_proactormetrics.cpp>
#include <ntci_proactorpool.cpp>
#include <ntci_proactorsocket.cpp>
#include <ntci_ratelimiter.cpp>
#include <ntci_ratelimiterfactory.cpp>
#include <ntci_reactor.cpp>
#include <ntci_reactorfactory.cpp>
#include <ntci_reactormetrics.cpp>
#include <ntci_reactorpool.cpp>
#include <ntci_reactorsocket.cpp>
#include <ntci_receivecallback.cpp>
#include <ntci_receivecallbackfactory.cpp>
#include <ntci_receivefuture.cpp>
#include <ntci_receiver.cpp>
#include <ntci_receiveresult.cpp>
#include <ntci_reservation.cpp>
#include <ntci_resolver.cpp>
#include <ntci_resolverfactory.cpp>
#include <ntci_scheduler.cpp>
#include <ntci_sendcallback.cpp>
#include <ntci_sendcallbackfactory.cpp>
#include <ntci_sender.cpp>
#include <ntci_sendfuture.cpp>
#include <ntci_sendresult.cpp>
#include <ntci_serialization.cpp>
#include <ntci_strand.cpp>
#include <ntci_strandfactory.cpp>
#include <ntci_streamsocket.cpp>
#include <ntci_streamsocketfactory.cpp>
#include <ntci_streamsocketmanager.cpp>
#include <ntci_streamsocketsession.cpp>
#include <ntci_thread.cpp>
#include <ntci_threadfactory.cpp>
#include <ntci_threadpool.cpp>
#include <ntci_timer.cpp>
#include <ntci_timercallback.cpp>
#include <ntci_timercallbackfactory.cpp>
#include <ntci_timerfactory.cpp>
#include <ntci_timerfuture.cpp>
#include <ntci_timerresult.cpp>
#include <ntci_timersession.cpp>
#include <ntci_upgradable.cpp>
#include <ntci_upgradecallback.cpp>
#include <ntci_upgradecallbackfactory.cpp>
#include <ntci_upgradefuture.cpp>
#include <ntci_upgraderesult.cpp>
#include <ntci_user.cpp>
#include <ntci_waiter.cpp>

// ntcs

#include <ntcs_async.cpp>
#include <ntcs_authorization.cpp>
#include <ntcs_blobbufferfactory.cpp>
#include <ntcs_blobbufferutil.cpp>
#include <ntcs_blobutil.cpp>
#include <ntcs_callbackstate.cpp>
#include <ntcs_chronology.cpp>
#include <ntcs_compat.cpp>
#include <ntcs_controller.cpp>
#include <ntcs_datapool.cpp>
#include <ntcs_detachstate.cpp>
#include <ntcs_dispatch.cpp>
#include <ntcs_driver.cpp>
#include <ntcs_event.cpp>
#include <ntcs_flowcontrolcontext.cpp>
#include <ntcs_flowcontrolstate.cpp>
#include <ntcs_global.cpp>
#include <ntcs_globalallocator.cpp>
#include <ntcs_globalexecutor.cpp>
#include <ntcs_interest.cpp>
#include <ntcs_leakybucket.cpp>
#include <ntcs_memorymap.cpp>
#include <ntcs_metrics.cpp>
#include <ntcs_monitorable.cpp>
#include <ntcs_nomenclature.cpp>
#include <ntcs_observer.cpp>
#include <ntcs_openstate.cpp>
#include <ntcs_plugin.cpp>
#include <ntcs_proactordetachcontext.cpp>
#include <ntcs_proactormetrics.cpp>
#include <ntcs_processmetrics.cpp>
#include <ntcs_processstatistics.cpp>
#include <ntcs_processutil.cpp>
#include <ntcs_ratelimiter.cpp>
#include <ntcs_reactormetrics.cpp>
#include <ntcs_registry.cpp>
#include <ntcs_reservation.cpp>
#include <ntcs_shutdowncontext.cpp>
#include <ntcs_shutdownstate.cpp>
#include <ntcs_skiplist.cpp>
#include <ntcs_strand.cpp>
#include <ntcs_threadutil.cpp>
#include <ntcs_user.cpp>
#include <ntcs_watermarks.cpp>
#include <ntcs_watermarkutil.cpp>

// ntcq

#include <ntcq_accept.cpp>
#include <ntcq_bind.cpp>
#include <ntcq_connect.cpp>
#include <ntcq_receive.cpp>
#include <ntcq_send.cpp>
#include <ntcq_zerocopy.cpp>

// ntcd

#include <ntcd_blobbufferfactory.cpp>
#include <ntcd_compression.cpp>
#include <ntcd_datagramsocket.cpp>
#include <ntcd_datapool.cpp>
#include <ntcd_datautil.cpp>
#include <ntcd_encryption.cpp>
#include <ntcd_listenersocket.cpp>
#include <ntcd_machine.cpp>
#include <ntcd_proactor.cpp>
#include <ntcd_reactor.cpp>
#include <ntcd_resolver.cpp>
#include <ntcd_simulation.cpp>
#include <ntcd_streamsocket.cpp>
#include <ntcd_test.cpp>
#include <ntcd_timer.cpp>

// ntcdns

#include <ntcdns_cache.cpp>
#include <ntcdns_client.cpp>
#include <ntcdns_compat.cpp>
#include <ntcdns_database.cpp>
#include <ntcdns_protocol.cpp>
#include <ntcdns_resolver.cpp>
#include <ntcdns_server.cpp>
#include <ntcdns_system.cpp>
#include <ntcdns_utility.cpp>
#include <ntcdns_vocabulary.cpp>

// ntctlc

#include <ntctlc_plugin.cpp>

// ntctls

#include <ntctls_plugin.cpp>

// ntcu

#include <ntcu_datagramsocketeventqueue.cpp>
#include <ntcu_datagramsocketsession.cpp>
#include <ntcu_datagramsocketutil.cpp>
#include <ntcu_listenersocketeventqueue.cpp>
#include <ntcu_listenersocketsession.cpp>
#include <ntcu_listenersocketutil.cpp>
#include <ntcu_streamsocketeventqueue.cpp>
#include <ntcu_streamsocketsession.cpp>
#include <ntcu_streamsocketutil.cpp>
#include <ntcu_timestampcorrelator.cpp>

// ntcr

#include <ntcr_datagramsocket.cpp>
#include <ntcr_interface.cpp>
#include <ntcr_listenersocket.cpp>
#include <ntcr_streamsocket.cpp>
#include <ntcr_thread.cpp>

// ntcp

#include <ntcp_datagramsocket.cpp>
#include <ntcp_interface.cpp>
#include <ntcp_listenersocket.cpp>
#include <ntcp_streamsocket.cpp>
#include <ntcp_thread.cpp>

// ntco

#include <ntco_devpoll.cpp>
#include <ntco_epoll.cpp>
#include <ntco_eventport.cpp>
#include <ntco_iocp.cpp>
#include <ntco_ioring.cpp>
#include <ntco_kqueue.cpp>
#include <ntco_poll.cpp>
#include <ntco_pollset.cpp>
#include <ntco_select.cpp>
#include <ntco_test.cpp>

// ntcf

#include <ntcf_api.cpp>
#include <ntcf_system.cpp>
#include <ntcf_test.cpp>

