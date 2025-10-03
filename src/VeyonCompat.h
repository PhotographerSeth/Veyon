#pragma once

// Compatibility helpers to include Veyon headers across layout changes.
// We prefer the namespaced include paths used in newer versions and fall back
// to the legacy flat headers when necessary.

#if __has_include(<core/Feature.h>)
#  include <core/Feature.h>
#elif __has_include(<Feature.h>)
#  include <Feature.h>
#else
#  error "Unable to locate Veyon Feature.h. Please set VEYON_SRC_DIR to the Veyon sources."
#endif

#if __has_include(<core/FeatureMessage.h>)
#  include <core/FeatureMessage.h>
#elif __has_include(<FeatureMessage.h>)
#  include <FeatureMessage.h>
#else
#  error "Unable to locate Veyon FeatureMessage.h. Please set VEYON_SRC_DIR to the Veyon sources."
#endif

#if __has_include(<core/FeatureProviderInterface.h>)
#  include <core/FeatureProviderInterface.h>
#elif __has_include(<plugins/FeatureProviderInterface.h>)
#  include <plugins/FeatureProviderInterface.h>
#elif __has_include(<FeatureProviderInterface.h>)
#  include <FeatureProviderInterface.h>
#else
#  error "Unable to locate Veyon FeatureProviderInterface.h. Please set VEYON_SRC_DIR to the Veyon sources."
#endif

#if __has_include(<common/PluginInterface.h>)
#  include <common/PluginInterface.h>
#elif __has_include(<core/PluginInterface.h>)
#  include <core/PluginInterface.h>
#elif __has_include(<PluginInterface.h>)
#  include <PluginInterface.h>
#else
#  error "Unable to locate Veyon PluginInterface.h. Please set VEYON_SRC_DIR to the Veyon sources."
#endif

#if __has_include(<server/VeyonServerInterface.h>)
#  include <server/VeyonServerInterface.h>
#elif __has_include(<master/VeyonServerInterface.h>)
#  include <master/VeyonServerInterface.h>
#elif __has_include(<VeyonServerInterface.h>)
#  include <VeyonServerInterface.h>
#else
#  error "Unable to locate Veyon VeyonServerInterface.h. Please set VEYON_SRC_DIR to the Veyon sources."
#endif

#if __has_include(<worker/VeyonWorkerInterface.h>)
#  include <worker/VeyonWorkerInterface.h>
#elif __has_include(<service/VeyonWorkerInterface.h>)
#  include <service/VeyonWorkerInterface.h>
#elif __has_include(<VeyonWorkerInterface.h>)
#  include <VeyonWorkerInterface.h>
#else
#  error "Unable to locate Veyon VeyonWorkerInterface.h. Please set VEYON_SRC_DIR to the Veyon sources."
#endif

#if __has_include(<master/ComputerControlInterface.h>)
#  include <master/ComputerControlInterface.h>
#elif __has_include(<ComputerControlInterface.h>)
#  include <ComputerControlInterface.h>
#else
#  error "Unable to locate Veyon ComputerControlInterface.h. Please set VEYON_SRC_DIR to the Veyon sources."
#endif

