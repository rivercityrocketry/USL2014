//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by a tool.
//     Runtime Version:4.0.30319.1008
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------
using System;
using System.ServiceModel;

namespace AssemblyCSharp
{
	[ServiceContract]
	public interface ISimulationUpdater
	{
		[OperationContract]
		void UpdateSim(float accelX, float accelY, float accelZ);
	}

	public class SimulationUpdater : ISimulationUpdater
	{
		public void UpdateSim(float accelX, float accelY, float accelZ)
		{
			Console.Out.WriteLine(accelX.ToString() + accelY.ToString() + accelZ.ToString());
		}
	}
}

