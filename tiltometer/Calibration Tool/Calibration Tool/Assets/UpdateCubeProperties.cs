using UnityEngine;
using UnityEditor;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Timers;

public class UpdateCubeProperties : MonoBehaviour {

	private const string IN_FILE_PATH = "C:\\Users\\Daniel\\Desktop\\USLI\\2013-2014\\codeRepository\\USL2014\\tiltometer\\Calibration Tool\\Calibration Tool\\log.txt";

	private float currentAccelX;
	private float currentAccelY;
	private float currentAccelZ;
	private bool animationPlayed = false;
	private AssemblyCSharp.Server Server;

	Timer timer = new Timer(2000);

	// Use this for initialization
	void Start () {
		Application.targetFrameRate = 1;

		timer.Elapsed += new ElapsedEventHandler(timer_Elapsed);
		timer.Enabled = true;
		timer.Start();

		this.Server = new AssemblyCSharp.Server();

		EditorApplication.playmodeStateChanged = HandleOnPlayModeChanged;
	}
	
	// Update is called once per frame
	void Update () {
		this.transform.Rotate(new Vector3(currentAccelX, currentAccelY, currentAccelZ));
	}

	void timer_Elapsed(object sender, ElapsedEventArgs e) {
		if (animationPlayed) {
			currentAccelX = 0.0f;
			currentAccelY = 0.0f;
			currentAccelZ = 0.0f;
		}
		else {
			animationPlayed = true;

			string[] lines = File.ReadAllLines(IN_FILE_PATH);

			foreach (string line in lines) {
				if (line != "") {
					Debug.Log(line);

					string data = line.Substring(1, line.Length - 3);
					string[] dataComponents = data.Split(',');

					if (dataComponents.Count() > 0) {
						currentAccelX = float.Parse(dataComponents[0]);
						currentAccelY = float.Parse(dataComponents[1]);
						currentAccelZ = float.Parse(dataComponents[2]);
	//					float gyroX  = float.Parse(dataComponents[3]);
						//					float gyroY  = float.Parse(dataComponents[4]);
						//float gyroZ  = float.Parse(dataComponents[5]);
						Debug.Log("Got the components");
						Debug.Log("Accel: [" + currentAccelX + "," + currentAccelY + "," + currentAccelZ + "]");
						//Debug.Log("Gyro: [" + gyroX + "," + gyroY + "," + gyroZ + "]");
					}
				}
			}
		}

		// Delete all the data from the file that we read in.
		Debug.Log("Before deletion");
		File.Delete(IN_FILE_PATH);
		Debug.Log("File deleted");
	}

	/// <summary>
	/// Stop the program, close the reader, etc.
	/// </summary>
	void HandleOnPlayModeChanged()
	{
		// Check if the editor is not playing or paused (=> we're not editing).
		if (!EditorApplication.isPaused && !EditorApplication.isPlaying)
		{
			timer.Stop();
			timer.Dispose();
			this.Server.Close();
		}
	}
}
