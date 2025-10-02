#pragma once

#include "FeatureMessage.h"

class VeyonWorkerInterface
{
public:
    void sendFeatureMessage(const FeatureMessage& message)
    {
        (void)message;
    }
};

