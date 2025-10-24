#include <ntccfg_config.h>

#if NTC_BUILD_UNITY == 0
#error Not expected
#endif

// ntcscm

// #include <ntcscm_version.t.cpp>
// #include <ntcscm_versiontag.t.cpp>

// ntccfg

#include <ntccfg_bind.t.cpp>
#include <ntccfg_bit.t.cpp>
#include <ntccfg_foreach.t.cpp>
#include <ntccfg_format.t.cpp>
#include <ntccfg_function.t.cpp>
#include <ntccfg_inline.t.cpp>
#include <ntccfg_likely.t.cpp>
#include <ntccfg_limits.t.cpp>
#include <ntccfg_lock.t.cpp>
#include <ntccfg_mutex.t.cpp>
#include <ntccfg_object.t.cpp>
#include <ntccfg_platform.t.cpp>
#include <ntccfg_traits.t.cpp>
#include <ntccfg_tune.t.cpp>

// ntca

// #include <ntca_acceptcontext.t.cpp>
// #include <ntca_acceptevent.t.cpp>
// #include <ntca_accepteventtype.t.cpp>
// #include <ntca_acceptoptions.t.cpp>
// #include <ntca_acceptqueuecontext.t.cpp>
// #include <ntca_acceptqueueevent.t.cpp>
// #include <ntca_acceptqueueeventtype.t.cpp>
// #include <ntca_accepttoken.t.cpp>
// #include <ntca_bindcontext.t.cpp>
// #include <ntca_bindevent.t.cpp>
// #include <ntca_bindeventtype.t.cpp>
// #include <ntca_bindoptions.t.cpp>
// #include <ntca_bindtoken.t.cpp>
#include <ntca_checksum.t.cpp>
// #include <ntca_connectcontext.t.cpp>
// #include <ntca_connectevent.t.cpp>
// #include <ntca_connecteventtype.t.cpp>
// #include <ntca_connectoptions.t.cpp>
// #include <ntca_connecttoken.t.cpp>
// #include <ntca_datagramsocketevent.t.cpp>
// #include <ntca_datagramsocketeventtype.t.cpp>
// #include <ntca_datagramsocketoptions.t.cpp>
// #include <ntca_downgradecontext.t.cpp>
// #include <ntca_downgradeevent.t.cpp>
// #include <ntca_downgradeeventtype.t.cpp>
// #include <ntca_driverconfig.t.cpp>
// #include <ntca_drivermechanism.t.cpp>
// #include <ntca_encryptionauthentication.t.cpp>
#include <ntca_encryptioncertificate.t.cpp>
// #include <ntca_encryptioncertificateoptions.t.cpp>
// #include <ntca_encryptionclientoptions.t.cpp>
#include <ntca_encryptionkey.t.cpp>
// #include <ntca_encryptionkeyoptions.t.cpp>
// #include <ntca_encryptionkeytype.t.cpp>
// #include <ntca_encryptionmethod.t.cpp>
// #include <ntca_encryptionoptions.t.cpp>
// #include <ntca_encryptionresource.t.cpp>
// #include <ntca_encryptionresourcedescriptor.t.cpp>
// #include <ntca_encryptionresourceoptions.t.cpp>
// #include <ntca_encryptionresourcetype.t.cpp>
// #include <ntca_encryptionrole.t.cpp>
// #include <ntca_encryptionsecret.t.cpp>
// #include <ntca_encryptionserveroptions.t.cpp>
// #include <ntca_encryptionvalidation.t.cpp>
// #include <ntca_errorcontext.t.cpp>
// #include <ntca_errorevent.t.cpp>
// #include <ntca_erroreventtype.t.cpp>
// #include <ntca_flowcontrolmode.t.cpp>
// #include <ntca_flowcontroltype.t.cpp>
// #include <ntca_getdomainnamecontext.t.cpp>
// #include <ntca_getdomainnameevent.t.cpp>
// #include <ntca_getdomainnameeventtype.t.cpp>
// #include <ntca_getdomainnameoptions.t.cpp>
// #include <ntca_getendpointcontext.t.cpp>
// #include <ntca_getendpointevent.t.cpp>
// #include <ntca_getendpointeventtype.t.cpp>
// #include <ntca_getendpointoptions.t.cpp>
// #include <ntca_getipaddresscontext.t.cpp>
// #include <ntca_getipaddressevent.t.cpp>
// #include <ntca_getipaddresseventtype.t.cpp>
// #include <ntca_getipaddressoptions.t.cpp>
// #include <ntca_getportcontext.t.cpp>
// #include <ntca_getportevent.t.cpp>
// #include <ntca_getporteventtype.t.cpp>
// #include <ntca_getportoptions.t.cpp>
// #include <ntca_getservicenamecontext.t.cpp>
// #include <ntca_getservicenameevent.t.cpp>
// #include <ntca_getservicenameeventtype.t.cpp>
// #include <ntca_getservicenameoptions.t.cpp>
// #include <ntca_interfaceconfig.t.cpp>
// #include <ntca_listenersocketevent.t.cpp>
// #include <ntca_listenersocketeventtype.t.cpp>
// #include <ntca_listenersocketoptions.t.cpp>
// #include <ntca_loadbalancingoptions.t.cpp>
// #include <ntca_monitorablecollectorconfig.t.cpp>
// #include <ntca_monitorableregistryconfig.t.cpp>
// #include <ntca_proactorconfig.t.cpp>
// #include <ntca_ratelimiterconfig.t.cpp>
// #include <ntca_reactorconfig.t.cpp>
// #include <ntca_reactorcontext.t.cpp>
// #include <ntca_reactorevent.t.cpp>
// #include <ntca_reactoreventoptions.t.cpp>
// #include <ntca_reactoreventtrigger.t.cpp>
// #include <ntca_reactoreventtype.t.cpp>
// #include <ntca_readqueuecontext.t.cpp>
// #include <ntca_readqueueevent.t.cpp>
// #include <ntca_readqueueeventtype.t.cpp>
// #include <ntca_receivecontext.t.cpp>
// #include <ntca_receiveevent.t.cpp>
// #include <ntca_receiveeventtype.t.cpp>
// #include <ntca_receiveoptions.t.cpp>
// #include <ntca_receivetoken.t.cpp>
// #include <ntca_resolverconfig.t.cpp>
// #include <ntca_resolversource.t.cpp>
// #include <ntca_schedulerconfig.t.cpp>
// #include <ntca_sendcontext.t.cpp>
// #include <ntca_sendevent.t.cpp>
// #include <ntca_sendeventtype.t.cpp>
// #include <ntca_sendoptions.t.cpp>
// #include <ntca_sendtoken.t.cpp>
// #include <ntca_shutdowncontext.t.cpp>
// #include <ntca_shutdownevent.t.cpp>
// #include <ntca_shutdowneventtype.t.cpp>
// #include <ntca_streamsocketevent.t.cpp>
// #include <ntca_streamsocketeventtype.t.cpp>
// #include <ntca_streamsocketoptions.t.cpp>
// #include <ntca_threadconfig.t.cpp>
// #include <ntca_timercontext.t.cpp>
// #include <ntca_timerevent.t.cpp>
// #include <ntca_timereventtype.t.cpp>
#include <ntca_timeroptions.t.cpp>
// #include <ntca_upgradecontext.t.cpp>
// #include <ntca_upgradeevent.t.cpp>
// #include <ntca_upgradeeventtype.t.cpp>
// #include <ntca_upgradeoptions.t.cpp>
// #include <ntca_upgradetoken.t.cpp>
// #include <ntca_waiteroptions.t.cpp>
// #include <ntca_writequeuecontext.t.cpp>
// #include <ntca_writequeueevent.t.cpp>
// #include <ntca_writequeueeventtype.t.cpp>

// ntci

#include <ntci_acceptcallback.t.cpp>
#include <ntci_acceptcallbackfactory.t.cpp>
#include <ntci_acceptfuture.t.cpp>
#include <ntci_acceptor.t.cpp>
#include <ntci_acceptresult.t.cpp>
// #include <ntci_authorization.t.cpp>
// #include <ntci_bindable.t.cpp>
// #include <ntci_bindcallback.t.cpp>
// #include <ntci_bindcallbackfactory.t.cpp>
// #include <ntci_bindfuture.t.cpp>
// #include <ntci_bindresult.t.cpp>
#include <ntci_callback.t.cpp>
// #include <ntci_cancellation.t.cpp>
// #include <ntci_chronology.t.cpp>
// #include <ntci_clock.t.cpp>
// #include <ntci_closable.t.cpp>
// #include <ntci_closecallback.t.cpp>
// #include <ntci_closecallbackfactory.t.cpp>
// #include <ntci_closefuture.t.cpp>
// #include <ntci_closeresult.t.cpp>
// #include <ntci_compression.t.cpp>
// #include <ntci_compressiondriver.t.cpp>
// #include <ntci_connectcallback.t.cpp>
// #include <ntci_connectcallbackfactory.t.cpp>
// #include <ntci_connectfuture.t.cpp>
// #include <ntci_connector.t.cpp>
// #include <ntci_connectresult.t.cpp>
#include <ntci_concurrent.t.cpp>
// #include <ntci_datagramsocket.t.cpp>
// #include <ntci_datagramsocketfactory.t.cpp>
// #include <ntci_datagramsocketmanager.t.cpp>
// #include <ntci_datagramsocketsession.t.cpp>
// #include <ntci_datapool.t.cpp>
// #include <ntci_driver.t.cpp>
// #include <ntci_driverfactory.t.cpp>
// #include <ntci_encryption.t.cpp>
// #include <ntci_encryptioncertificate.t.cpp>
// #include <ntci_encryptioncertificategenerator.t.cpp>
// #include <ntci_encryptioncertificatestorage.t.cpp>
// #include <ntci_encryptionclient.t.cpp>
// #include <ntci_encryptionclientfactory.t.cpp>
// #include <ntci_encryptiondriver.t.cpp>
// #include <ntci_encryptionkey.t.cpp>
// #include <ntci_encryptionkeygenerator.t.cpp>
// #include <ntci_encryptionkeystorage.t.cpp>
// #include <ntci_encryptionresource.t.cpp>
// #include <ntci_encryptionresourcefactory.t.cpp>
// #include <ntci_encryptionserver.t.cpp>
// #include <ntci_encryptionserverfactory.t.cpp>
// #include <ntci_executor.t.cpp>
// #include <ntci_getdomainnamecallback.t.cpp>
// #include <ntci_getdomainnamecallbackfactory.t.cpp>
// #include <ntci_getendpointcallback.t.cpp>
// #include <ntci_getendpointcallbackfactory.t.cpp>
// #include <ntci_getipaddresscallback.t.cpp>
// #include <ntci_getipaddresscallbackfactory.t.cpp>
// #include <ntci_getportcallback.t.cpp>
// #include <ntci_getportcallbackfactory.t.cpp>
// #include <ntci_getservicenamecallback.t.cpp>
// #include <ntci_getservicenamecallbackfactory.t.cpp>
#include <ntci_identifiable.t.cpp>
// #include <ntci_interface.t.cpp>
// #include <ntci_invoker.t.cpp>
// #include <ntci_listenersocket.t.cpp>
// #include <ntci_listenersocketfactory.t.cpp>
// #include <ntci_listenersocketmanager.t.cpp>
// #include <ntci_listenersocketsession.t.cpp>
#include <ntci_log.t.cpp>
#include <ntci_metric.t.cpp>
#include <ntci_monitorable.t.cpp>
#include <ntci_mutex.t.cpp>
// #include <ntci_proactor.t.cpp>
// #include <ntci_proactorfactory.t.cpp>
// #include <ntci_proactormetrics.t.cpp>
// #include <ntci_proactorpool.t.cpp>
// #include <ntci_proactorsocket.t.cpp>
// #include <ntci_ratelimiter.t.cpp>
// #include <ntci_ratelimiterfactory.t.cpp>
// #include <ntci_reactor.t.cpp>
// #include <ntci_reactorfactory.t.cpp>
// #include <ntci_reactormetrics.t.cpp>
// #include <ntci_reactorpool.t.cpp>
// #include <ntci_reactorsocket.t.cpp>
// #include <ntci_receivecallback.t.cpp>
// #include <ntci_receivecallbackfactory.t.cpp>
// #include <ntci_receivefuture.t.cpp>
// #include <ntci_receiver.t.cpp>
// #include <ntci_receiveresult.t.cpp>
// #include <ntci_reservation.t.cpp>
// #include <ntci_resolver.t.cpp>
// #include <ntci_resolverfactory.t.cpp>
// #include <ntci_scheduler.t.cpp>
// #include <ntci_sendcallback.t.cpp>
// #include <ntci_sendcallbackfactory.t.cpp>
// #include <ntci_sender.t.cpp>
// #include <ntci_sendfuture.t.cpp>
// #include <ntci_sendresult.t.cpp>
// #include <ntci_strand.t.cpp>
// #include <ntci_strandfactory.t.cpp>
// #include <ntci_streamsocket.t.cpp>
// #include <ntci_streamsocketfactory.t.cpp>
// #include <ntci_streamsocketmanager.t.cpp>
// #include <ntci_streamsocketsession.t.cpp>
// #include <ntci_thread.t.cpp>
// #include <ntci_threadfactory.t.cpp>
// #include <ntci_threadpool.t.cpp>
// #include <ntci_timer.t.cpp>
// #include <ntci_timercallback.t.cpp>
// #include <ntci_timercallbackfactory.t.cpp>
// #include <ntci_timerfactory.t.cpp>
// #include <ntci_timerfuture.t.cpp>
// #include <ntci_timerresult.t.cpp>
// #include <ntci_timersession.t.cpp>
// #include <ntci_upgradable.t.cpp>
// #include <ntci_upgradecallback.t.cpp>
// #include <ntci_upgradecallbackfactory.t.cpp>
// #include <ntci_user.t.cpp>
// #include <ntci_waiter.t.cpp>

// ntcs

#include <ntcs_async.t.cpp>
#include <ntcs_authorization.t.cpp>
#include <ntcs_blobbufferfactory.t.cpp>
#include <ntcs_blobbufferutil.t.cpp>
#include <ntcs_blobutil.t.cpp>
#include <ntcs_callbackstate.t.cpp>
#include <ntcs_chronology.t.cpp>
#include <ntcs_compat.t.cpp>
#include <ntcs_controller.t.cpp>
#include <ntcs_datapool.t.cpp>
#include <ntcs_detachstate.t.cpp>
#include <ntcs_dispatch.t.cpp>
#include <ntcs_driver.t.cpp>
#include <ntcs_event.t.cpp>
#include <ntcs_flowcontrolcontext.t.cpp>
#include <ntcs_flowcontrolstate.t.cpp>
#include <ntcs_global.t.cpp>
#include <ntcs_globalallocator.t.cpp>
#include <ntcs_globalexecutor.t.cpp>
#include <ntcs_interactable.t.cpp>
#include <ntcs_interest.t.cpp>
#include <ntcs_leakybucket.t.cpp>
#include <ntcs_memorymap.t.cpp>
#include <ntcs_metrics.t.cpp>
#include <ntcs_monitorable.t.cpp>
#include <ntcs_nomenclature.t.cpp>
#include <ntcs_observer.t.cpp>
#include <ntcs_openstate.t.cpp>
#include <ntcs_plugin.t.cpp>
#include <ntcs_proactordetachcontext.t.cpp>
#include <ntcs_proactormetrics.t.cpp>
#include <ntcs_processmetrics.t.cpp>
#include <ntcs_processstatistics.t.cpp>
#include <ntcs_processutil.t.cpp>
#include <ntcs_ratelimiter.t.cpp>
#include <ntcs_reactormetrics.t.cpp>
#include <ntcs_registry.t.cpp>
#include <ntcs_reservation.t.cpp>
#include <ntcs_shutdowncontext.t.cpp>
#include <ntcs_shutdownstate.t.cpp>
#include <ntcs_skiplist.t.cpp>
#include <ntcs_strand.t.cpp>
#include <ntcs_threadutil.t.cpp>
#include <ntcs_user.t.cpp>
#include <ntcs_watermarks.t.cpp>
#include <ntcs_watermarkutil.t.cpp>

// ntcq

#include <ntcq_accept.t.cpp>
#include <ntcq_bind.t.cpp>
#include <ntcq_connect.t.cpp>
#include <ntcq_receive.t.cpp>
#include <ntcq_send.t.cpp>
#include <ntcq_zerocopy.t.cpp>

// ntcd

#include <ntcd_blobbufferfactory.t.cpp>
#include <ntcd_compression.t.cpp>
#include <ntcd_datagramsocket.t.cpp>
#include <ntcd_datapool.t.cpp>
#include <ntcd_datautil.t.cpp>
#include <ntcd_encryption.t.cpp>
#include <ntcd_listenersocket.t.cpp>
#include <ntcd_machine.t.cpp>
#include <ntcd_proactor.t.cpp>
#include <ntcd_reactor.t.cpp>
#include <ntcd_resolver.t.cpp>
#include <ntcd_simulation.t.cpp>
#include <ntcd_streamsocket.t.cpp>
#include <ntcd_test.t.cpp>
#include <ntcd_timer.t.cpp>

// ntcdns

#include <ntcdns_cache.t.cpp>
#include <ntcdns_client.t.cpp>
#include <ntcdns_compat.t.cpp>
#include <ntcdns_database.t.cpp>
#include <ntcdns_protocol.t.cpp>
#include <ntcdns_resolver.t.cpp>
#include <ntcdns_server.t.cpp>
#include <ntcdns_system.t.cpp>
#include <ntcdns_utility.t.cpp>
#include <ntcdns_vocabulary.t.cpp>

// ntctlc

#include <ntctlc_plugin.t.cpp>

// ntctls

#include <ntctls_plugin.t.cpp>

// ntcu

#include <ntcu_datagramsocketeventqueue.t.cpp>
#include <ntcu_datagramsocketsession.t.cpp>
#include <ntcu_datagramsocketutil.t.cpp>
#include <ntcu_listenersocketeventqueue.t.cpp>
#include <ntcu_listenersocketsession.t.cpp>
#include <ntcu_listenersocketutil.t.cpp>
#include <ntcu_streamsocketeventqueue.t.cpp>
#include <ntcu_streamsocketsession.t.cpp>
#include <ntcu_streamsocketutil.t.cpp>
#include <ntcu_timestampcorrelator.t.cpp>

// ntcr

#include <ntcr_datagramsocket.t.cpp>
#include <ntcr_interface.t.cpp>
#include <ntcr_listenersocket.t.cpp>
#include <ntcr_streamsocket.t.cpp>
#include <ntcr_thread.t.cpp>

// ntcp

#include <ntcp_datagramsocket.t.cpp>
#include <ntcp_interface.t.cpp>
#include <ntcp_listenersocket.t.cpp>
#include <ntcp_streamsocket.t.cpp>
#include <ntcp_thread.t.cpp>

// ntco

#include <ntco_devpoll.t.cpp>
#include <ntco_epoll.t.cpp>
#include <ntco_eventport.t.cpp>
#include <ntco_iocp.t.cpp>
#include <ntco_ioring.t.cpp>
#include <ntco_kqueue.t.cpp>
#include <ntco_poll.t.cpp>
#include <ntco_pollset.t.cpp>
#include <ntco_select.t.cpp>
#include <ntco_test.t.cpp>

// ntcf

// #include <ntcf_api.t.cpp>
#include <ntcf_system.t.cpp>
#include <ntcf_test.t.cpp>

