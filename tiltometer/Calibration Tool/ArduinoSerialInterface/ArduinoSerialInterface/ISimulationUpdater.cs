using System;
using System.ServiceModel;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ArduinoSerialInterface
{
    [ServiceContract]
    public interface ISimulationUpdater
    {
        [OperationContract]
        void UpdateSim(float accelX, float accelY, float accelZ);
    }
}
