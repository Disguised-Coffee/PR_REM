import Image from "next/image";
import axios from 'axios';
// import Particle from 'particle-api-js';

export default function Home() {
  // handleRecordData();
  sendMessageData();
  return (
    <div>
      <h1>Home</h1>

    </div>
  );
}

function tableData() {
  return (
    <div>

    </div>
  );
}

function handleRecordData() {
  const apiURL = `https://script.google.com/macros/s/${process.env.APPSCRIPT_KEY}/exec?beginDate=1-8-25&endDate=1-9-25&limit=2`;
  axios.get(apiURL).then((res) => {
    console.log(res.data);
  }).catch((err) => {
    console.log(err);
    // might want to do something like a banner later []
  });
}

/**
 * Unholy voodoo magic was used to get this to work
 * 
 * (CoPilot decompiled a library [thanks CoPilot])
 */
function sendMessageData() {
  const apiURL = `https://api.particle.io/v1/devices/events/`;

  const data = {
    name: `${process.env.PART_EVENT}`,
    data: JSON.stringify({ msg: 'booooooooo' }),
    private: true
  };
  const headers = {
    'Authorization': `Bearer ${process.env.PART_AT}`,
    'Content-Type': 'application/json'
  };

  axios.post(apiURL, data, {headers}).then((res) => {
    console.log(res.data);
  }).catch((err) => {
    console.log(err);
    // might want to do something like a banner later []
  });
}
