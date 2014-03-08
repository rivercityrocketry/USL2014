using UnityEngine;
using UnityEditor;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System;
using System.IO.Ports;
using System.Linq;
using System.Timers;

public class UpdateCubeProperties : MonoBehaviour {

	private const string IN_FILE_PATH = "C:\\Users\\Daniel\\Desktop\\USLI\\2013-2014\\codeRepository\\USL2014\\tiltometer\\Calibration Tool\\Calibration Tool\\log.txt";
	private const int BAUD_RATE = 115200;

	private string mostRecentReading = "";

	private SerialPort SerialStream;

	private Vector3 previousAngles = Vector3.zero;

	// Use this for initialization
	void Start () {
		EditorApplication.playmodeStateChanged = HandleOnPlayModeChanged;

		SerialStream = new SerialPort("\\\\.\\COM10", BAUD_RATE);
	//	SerialStream.Handshake = Handshake.RequestToSend;
	//	SerialStream.DataReceived += new SerialDataReceivedEventHandler(DataReceivedHandler);
		SerialStream.Open();
	}
	
	// Update is called once per frame
	void Update () {
		try {
			if (SerialStream != null && SerialStream.IsOpen) {
    			string text = SerialStream.ReadLine();
				float xrot, yrot, zrot;

				if (text.Length >= "euler\t".Length && text.StartsWith("euler")) {
    				text = text.Substring("euler\t".Length);

			    	string[] components = text.Split(new char[]{'\t'});
	
					if (   components.Length == 3
				    	&& float.TryParse(components[0], out yrot)
				    	&& float.TryParse(components[1], out zrot)
				    	&& float.TryParse(components[2], out xrot)) {

						Vector3 mostRecentOrientation = new Vector3(xrot, yrot, zrot);
						mostRecentOrientation += new Vector3(180.0f, 180.0f, 180.0f);
						mostRecentReading = mostRecentOrientation.ToString();
						transform.eulerAngles = mostRecentOrientation;
						//transform.eulerAngles = new Vector3(xrot, yrot, zrot);
/*
						transform.Rotate(new Vector3((xrot - previousAngles.x),
						                             (yrot - previousAngles.y), 
						                             (zrot - previousAngles.z)));*/
				
						//previousAngles = new Vector3(xrot, yrot, zrot);
					}
				}
				else {
			    	if (SerialStream != null && !SerialStream.IsOpen) {
				    	SerialStream.Close();
					    EditorApplication.isPlaying = false;
				    }
			    }
			}
	    }
 	    catch (Exception ex) {
		    Debug.Log(ex.ToString());
	    }
    }

	void OnGUI() {
		GUI.Label(new Rect(10,0,300,100), "Most recent reading: " + mostRecentReading);
		GUI.Label(new Rect(10,70,300,100), "Actual: " + transform.localEulerAngles.ToString());
	}

	/// <summary>
	/// Stop the program, close the reader, etc.
	/// </summary>
	void HandleOnPlayModeChanged()
	{
		// Check if the editor is not playing or paused (=> we're not editing).
		if (!EditorApplication.isPaused && !EditorApplication.isPlaying && SerialStream != null)
		{
			SerialStream.Close();
		}
	}

/*	private void DataReceivedHandler(object sender, SerialDataReceivedEventArgs e) {
		if (SerialStream.IsOpen) {
			string text = SerialStream.ReadLine();
			float xrot, yrot, zrot;
			
			text = text.Substring("ypr ".Length);
			
			string[] components = text.Split(new char[]{'\t'});
			
			if (   components.Length == 3 
			    && float.TryParse(components[0], out xrot)
			    && float.TryParse(components[1], out yrot)
			    && float.TryParse(components[2], out zrot)) {
				
				transform.Rotate(new Vector3((xrot - previousAngles.x),
				                             (yrot - previousAngles.y), 
				                             (zrot - previousAngles.z)));
				previousAngles = new Vector3(xrot, yrot, zrot);
			}
		}
		else {
			SerialStream.Close();
		}
		/*string indata = sp.ReadExisting();
		Console.WriteLine("Data Received:");
		Console.Write(indata);*/
	//}
}