/*
  Copyright (c) DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#include "dse.h"

#include "dse_auth.hpp"
#include "cluster_config.hpp"
#include "string.hpp"

using namespace datastax;
using namespace datastax::internal::enterprise;

static void dse_plaintext_authenticator_cleanup(void* data) {
  delete static_cast<PlaintextAuthenticatorData*>(data);
}

static void dse_gssapi_authenticator_cleanup(void* data) {
  delete static_cast<GssapiAuthenticatorData*>(data);
}

extern "C" {

CassCluster* cass_cluster_new_dse() {
  CassCluster* cluster = cass_cluster_new();
  cluster->config().set_host_targeting(true);
  return cluster;
}

CassError cass_cluster_set_dse_plaintext_authenticator(CassCluster* cluster,
                                                       const char* username,
                                                       const char* password) {
  return cass_cluster_set_dse_plaintext_authenticator_n(cluster,
                                                        username, SAFE_STRLEN(username),
                                                        password, SAFE_STRLEN(password));
}

CassError cass_cluster_set_dse_plaintext_authenticator_n(CassCluster* cluster,
                                                         const char* username, size_t username_length,
                                                         const char* password, size_t password_length) {
  return cass_cluster_set_dse_plaintext_authenticator_proxy_n(cluster,
                                                              username, username_length,
                                                              password, password_length,
                                                              "", 0);
}

CassError cass_cluster_set_dse_plaintext_authenticator_proxy(CassCluster* cluster,
                                                             const char* username,
                                                             const char* password,
                                                             const char* authorization_id) {
  return cass_cluster_set_dse_plaintext_authenticator_proxy_n(cluster,
                                                              username, SAFE_STRLEN(username),
                                                              password, SAFE_STRLEN(password),
                                                              authorization_id, SAFE_STRLEN(authorization_id));
}

CassError cass_cluster_set_dse_plaintext_authenticator_proxy_n(CassCluster* cluster,
                                                               const char* username, size_t username_length,
                                                               const char* password, size_t password_length,
                                                               const char* authorization_id, size_t authorization_id_length) {
  CassError rc = cass_cluster_set_authenticator_callbacks(cluster,
                                                          PlaintextAuthenticatorData::callbacks(),
                                                          dse_plaintext_authenticator_cleanup,
                                                          new PlaintextAuthenticatorData(String(username, username_length),
                                                                                         String(password, password_length),
                                                                                         String(authorization_id, authorization_id_length)));

  if (rc == CASS_OK) {
    String name = "DSEPlainTextAuthProvider";
    if (authorization_id_length > 0) {
      name.append(" (Proxy)");
    }
    cluster->config().auth_provider()->set_name(name);
  }

  return rc;
}

CassError cass_cluster_set_dse_gssapi_authenticator(CassCluster* cluster,
                                                    const char* service,
                                                    const char* principal) {
  return cass_cluster_set_dse_gssapi_authenticator_n(cluster,
                                                     service, SAFE_STRLEN(service),
                                                     principal, SAFE_STRLEN(principal));
}

CassError cass_cluster_set_dse_gssapi_authenticator_n(CassCluster* cluster,
                                                      const char* service, size_t service_length,
                                                      const char* principal, size_t principal_length) {
  return cass_cluster_set_dse_gssapi_authenticator_proxy_n(cluster,
                                                           service, service_length,
                                                           principal, principal_length,
                                                           "", 0);
}

CassError cass_cluster_set_dse_gssapi_authenticator_proxy(CassCluster* cluster,
                                                          const char* service,
                                                          const char* principal,
                                                          const char* authorization_id) {
  return cass_cluster_set_dse_gssapi_authenticator_proxy_n(cluster,
                                                           service, SAFE_STRLEN(service),
                                                           principal, SAFE_STRLEN(principal),
                                                           authorization_id, SAFE_STRLEN(authorization_id));
}

CassError cass_cluster_set_dse_gssapi_authenticator_proxy_n(CassCluster* cluster,
                                                            const char* service, size_t service_length,
                                                            const char* principal, size_t principal_length,
                                                            const char* authorization_id, size_t authorization_id_length) {
  CassError rc = cass_cluster_set_authenticator_callbacks(cluster,
                                                          GssapiAuthenticatorData::callbacks(),
                                                          dse_gssapi_authenticator_cleanup,
                                                          new GssapiAuthenticatorData(String(service, service_length),
                                                                                      String(principal, principal_length),
                                                                                      String(authorization_id, authorization_id_length)));
  if (rc == CASS_OK) {
    String name = "DSEGSSAPIAuthProvider";
    if (authorization_id_length > 0) {
      name.append(" (Proxy)");
    }
    cluster->config().auth_provider()->set_name(name);
  }

  return rc;
}

void cass_cluster_set_application_name(CassCluster* cluster,
                                       const char* application_name) {
  cass_cluster_set_application_name_n(cluster,
                                      application_name, SAFE_STRLEN(application_name));
}

void cass_cluster_set_application_name_n(CassCluster* cluster,
                                         const char* application_name,
                                         size_t application_name_length) {
  cluster->config().set_application_name(String(application_name,
                                                application_name_length));
}

void cass_cluster_set_application_version(CassCluster* cluster,
                                          const char* application_version) {
  cass_cluster_set_application_version_n(cluster,
                                         application_version, SAFE_STRLEN(application_version));
}

void cass_cluster_set_application_version_n(CassCluster* cluster,
                                            const char* application_version,
                                            size_t application_version_length) {
  cluster->config().set_application_version(String(application_version,
                                                   application_version_length));
}

void cass_cluster_set_client_id(CassCluster* cluster, CassUuid client_id) {
  cluster->config().set_client_id(client_id);
}

void cass_cluster_set_monitor_reporting_interval(CassCluster* cluster,
                                                 unsigned interval_secs) {
  cluster->config().set_monitor_reporting_interval_secs(interval_secs);
}

} // extern "C"
