"use client"
import axios from 'axios';
import { useState } from 'react';

export default function Home() {

  let [error, setError] = useState(null);

  /**
 * Unholy voodoo magic was used to get this to work
 * 
 * (CoPilot decompiled a library [thanks CoPilot])
 */
  function sendMessageData(message) {
    const apiURL = `https://api.particle.io/v1/devices/events/`;

    const data = {
      name: `${process.env.PART_EVENT}`,
      data: JSON.stringify({ msg: message }),
      private: true
    };
    const headers = {
      'Authorization': `Bearer ${process.env.PART_AT}`,
      'Content-Type': 'application/json'
    };

    axios.post(apiURL, data, { headers }).then((res) => {
      console.log(res.data);
    }).catch((err) => {
      setError(err);
      // might want to do something like a banner later []
    });
  }
  function getRecordData(limit, beginDate, endDate) {
    const apiURL = beginDate && endDate ? `https://script.google.com/macros/s/${process.env.APPSCRIPT_KEY}/exec?beginDate=${beginDate}&endDate=${endDate}&limit=${limit}`
      : `https://script.google.com/macros/s/${process.env.APPSCRIPT_KEY}/exec?limit=${limit}`;
    axios.get(apiURL).then((res) => {
      return res.data;
    }).catch((err) => {
      setError(err);
    });
  }



  // handleRecordData();
  // sendMessageData();
  return (
    <div>
      <NavBar />
      <Container defaultStr="Container"/>



    </div>
  );
}

const NavBar = () => {

  const [projectName, setProjectName] = useState('Room Environment Module');

  const handleChange = (event) => {
    setProjectName(event.target.value);
  };

  return (
    <nav className='p-2 bg-primary text-black h-[8vh] absolute w-full'>
      <div className='absolute top-1/2 transform -translate-y-1/2'>
        <label className='text-xs absolute italic font-medium italic text-white text-shadow top-[-0.5vh]'
          
          htmlFor="textarea"
        >PR name:</label>
        <textarea
          className='pl-4 font-Ubuntu font-medium italic text-[2rem] resize-none bg-transparent h-[5vh] max-w-[30vw]  overflow-hidden'
          value={projectName}
          onChange={handleChange}
          rows="5" cols="33"
          contentEditable="true"
          name='textarea'
        />
      </div>
    </nav>
  );
}

const Container = (props) => {
  console.log(props.defaultStr);

  const [containerName, setcontainerName] = useState(props.defaultStr);

  const handleChange = (event) => {
    setcontainerName(event.target.value);
  };
  
  return (
    <div className='bg-cbackground custom-shadow text-black h-[32vh] w-[32vw] absolute top-[8vh] left-[8vw]'>
      {/* top area */}
      <div className='custom-border bg-cForeground 
                      h-[6vh]
                      flex justify-center items-center
                      w-[17vw]
                      absolute
                      right-5
                  '>
        <textarea
          className='pr-4 font-Ubuntu font-regular italic text-[2rem] resize-none bg-transparent h-[5vh] max-w-[30vw]  overflow-hidden text-right'
          value={containerName}
          onChange={handleChange}
          rows="5" cols="33"
          contentEditable="true"
          name='textarea'
          />
      </div>
      {/* content */}
      <div className='mt-[6vh] p-2'>
        lorem ipsum
      </div>
    </div>
  );
}

function tableData() {
  return (
    <div>

    </div>
  );
}


