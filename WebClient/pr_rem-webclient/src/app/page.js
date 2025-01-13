"use client"
import axios from 'axios';
import React, { Children, useEffect, useState } from 'react';

export default function Home() {
  const APPSCRIPT_KEY = process.env.NEXT_PUBLIC_APPSCRIPT_KEY
  // console.info(process.env.NODE_ENV)
  // console.info(process.env)
  // console.info(process.env.APPSCRIPT_KEY)

  let [error, setError] = useState(null);

  let [queryObj, setQueryObj] = useState({
    limit: 5,
    beginDate: "1-5-25",
    endDate: "1-12-25",
    liveUpdate: false // Feature needs to be implemented before actual usage.
  });

  // finish sendMsg feature []
  const [msgResp, setMsgResp] = useState({});
  const [refreshVitals, setRefreshVitals] = useState(false)

  useEffect(() => {
    if(!msgResp){
      return null;
    }
    /**
     * Unholy voodoo magic was used to get this to work
     * 
     * (CoPilot decompiled a library [thanks CoPilot])
     */
    async function sendMessageData(message) {
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
        setMsgResp(res.data)
      }).catch((err) => {
        setError(err);
        // might want to do something like a banner later []
      });
    }
  }, [getVitals])




  // test data, with arrays
  const advData = {
    data: [
      {
        "Device ID": "haha very funny",
        "Post Date": "2025-01-08T23:15:56.347Z",
        "Temp": 0,
        "humidity": 18,
        "dewPt": 25.329565,
        "hi": 1.483619
      },
      {
        "Device ID": "someID",
        "Post Date": "2025-01-08T23:15:47.244Z",
        "Temp": 0,
        "humidity": 18,
        "dewPt": 25.451307,
        "hi": 1.900354
      },
    ]
  }

  const testJSON = [
    {
      "name": "John Doe",
      "date": "2025-01-08T23:15:56.347Z",
    },
    {
      "name": "John Doe",
      "date": "2025-01-08T23:15:56.347Z",
    },
    {
      "name": "John Doe",
      "date": "2025-01-08T23:15:56.347Z",
    }
  ]

  const [recordData, setRecordData] = useState(null);
  const [refresh, setRefresh] = useState(false);


  useEffect(() => {
    console.error("You forgot this statement 😉\n\tThis is a reminder that bugs are features");
    console.info("Sending Out!");
    async function getRecordData(limit, beginDate, endDate) {
      console.log(APPSCRIPT_KEY);
      const apiURL = beginDate && endDate ? `https://script.google.com/macros/s/${APPSCRIPT_KEY}/exec?beginDate=${beginDate}&endDate=${endDate}&limit=${limit}`
        : `https://script.google.com/macros/s/${APPSCRIPT_KEY}/exec?limit=${limit}`;
      console.log(apiURL);
      axios.get(apiURL).then((res) => {
        setRecordData(res.data);
      }).catch((err) => {
        setError(err);
      });
    }
    console.log(queryObj)
    getRecordData(queryObj.limit, queryObj.beginDate, queryObj.endDate);
  }, [refresh])

  // sendMessageData();
  return (
    <div>
      <NavBar />
      <div className='flex'>
        <div>
          <Container defaultStr="Container">
            <DisplayJSON data={testJSON} />
          </Container>
          <Container defaultStr="Query Set">
            <QueryScreen chRefresh={setRefresh} chObj={setQueryObj} />
          </Container>
          <Container defaultStr="Send a Message">
            {/* <SendAmsg chRefresh={ } /> */}
          </Container>

        </div>
        <LargeContainer json={recordData} />

      </div>



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
          spellCheck='false'
        />
      </div>
    </nav>
  );
}

const Container = (props) => {

  // []
  const [containerName, setcontainerName] = useState(props.defaultStr);

  // keeping track of pages on container
  // get back to this []
  const [page, setPage] = useState(0);

  const handleChange = (event) => {
    setcontainerName(event.target.value);
  };

  return (
    <div className='bg-cbackground custom-shadow text-black h-[32vh] w-[32vw] relative top-[8vh] left-[8vw]'>
      {/* top area */}
      <div className='custom-border bg-cForeground 
                      h-[6vh]
                      flex justify-center items-center
                      w-[17vw]
                      absolute
                      right-5
                  '>
        <textarea
          className='pr-4 font-Ubuntu font-regular text-[2rem] resize-none bg-transparent h-[5vh] max-w-[30vw]  overflow-hidden text-right'
          value={containerName}
          onChange={handleChange}
          rows="5" cols="33"
          contentEditable="true"
          spellCheck='false'
        />
      </div>
      {/* content */}
      <div className='mt-[6vh] p-2 pt-[7vh]'>
        {props.children}
      </div>
    </div>
  );
}


const DisplayJSON = (props) => {
  return (
    <div className='w-[31vw] h-[24vh]'>
      <div className='h-[2vh] bg-cForeground text-right text-xs font-IBM font-regular italic pr-2'>
        as RAW JSON
      </div>
      <div className='bg-white h-[22vh] overflow-y-scroll overflow-x-hidden'>
        <pre>
          {JSON.stringify(props.data, null, 2)}
        </pre>
      </div>
    </div>
  );
}

/**
 * Takes in a JSON object and returns a table
 * 
 * Uses the keys as the header and the values as the data
 * 
 * @returns JSX
 */
function LargeContainer(props) {
  // div table
  return (
    <div className='bg-cbackground custom-shadow text-black h-[82vh] w-[50vw] relative ml-[30px] top-[8vh] left-[8vw]'>
      {/* top area */}
      <div className='custom-border bg-cForeground 
                      h-[6vh]
                      flex justify-center items-center
                      w-[17vw]
                      absolute
                      right-5
                  '>
        <h1
          className='pr-4 font-Ubuntu font-regular text-[2rem] resize-none bg-transparent h-[5vh] max-w-[30vw]  overflow-hidden text-right'

        >Sensor Data
        </h1>
      </div>
      {/* content */}
      <div className='mt-[6vh] p-2'>
        <div className='h-[2vh] bg-cForeground text-right text-xs font-IBM font-regular italic pr-2'>
          as Div Table
        </div>
        <DisplayTable json={props.json} />
      </div>


    </div>
  );
}

function DisplayTable(props) {
  console.log(props.json)
  if (!props.json || props.json == null) {
    console.log("obj is stil null");
    return (
      <div className='bg-white h-[72vh]'>
        Data is still loading...
      </div>
    )
  }


  let headers = Object.keys(props.json.data[0]);

  if (!headers) {
    return (
      <div className='bg-white h-[72vh]'>
        No data!<br />
        Maybe it was a incorrect query?
      </div>
    )
  }

  const headerLen = headers.length + 1;

  return (
    <div className='bg-white h-[72vh] overflow-y-scroll overflow-x-hidden'>

      {/* note: use style in JSX to dynamically do stuff there rather than in tailwindCSS classes */}
      <div className='table' >
        {/* I wish the header could stay in place []... */}
        <div className="table-header-group">
          <div className="table-row">
            <div className='table-cell font-bold italic font-IBM p-2 bg-cShadow'>
              Instance
            </div>
            {headers.map((header, index) => {
              return (
                <div className="table-cell bg-cShadow font-bold italic font-IBM" key={index}>{header}</div>
              );
            })
            }
          </div>
        </div>
        {/* actual data */}
        <div className="table-row-group">
          {
            props.json.data.map((data, index) => {
              return (
                <div className="table-row" key={index}>
                  <div className='table-cell p-2 max-w-[1vw]'>
                    {index}
                  </div>
                  {
                    headers.map((header, index) => {
                      return (
                        <div className='table-cell p-2' key={index}>
                          {data[header]}
                        </div>
                      );
                    })
                  }
                </div>
              );
            })
          }
        </div>
      </div>
    </div>
  )
}


/**
 * 
 * @param chRefresh function to refresh data
 * 
 * 
 * @param chObj Change Handler of obj JSON obj
 * @return JSX
 */
function QueryScreen(props) {
  const [formData, setFormData] = useState({});

  // save the data to the JSON obj (our query)
  function handleSubmission(e) {
    e.preventDefault();

    props.chObj(formData);
  }

  function handleChange(e) {
    // setObj( value | callback(previous value))
    setFormData((prev) => {
      const updatedFormData = prev ?? { beginDate: "", endDate: "", limit: 0 }

      // append the data.
      return {
        ...updatedFormData,
        [e.target.name]: e.target.value,
      }
    }
    );
  }

  function handleRefresh() {
    props.chRefresh((prior) => {
      return !prior;
    })
  }


  return (
    <div>
      <form onSubmit={handleSubmission}>
        <div className='mb-2'>
          <label htmlFor='limit'>Max instances</label>
          <input className='ml-2 p-[0.25rem]' onChange={handleChange}
            name="limit"
          />
        </div>
        <div className='mb-2'>
          <label htmlFor='beginDate'>Min Date</label>
          <input className='ml-2 p-[0.25rem]' onChange={handleChange} name="beginDate" />
        </div>
        <div className='mb-2'>
          <label htmlFor='endDate'>Max Date</label>
          <input className='ml-2 p-[0.25rem]' onChange={handleChange} name='endDate' />
        </div>
        <div>
          {/* refresh btn */}
          {/* <button className="ml-2 bg-[#9AAEF6] rounded-sm shadow">Save settings</button> */}
          <button className="ml-2 bg-[#9AAEF6] rounded-sm shadow p-2" onClick={handleRefresh}>Refresh</button>
        </div>
      </form>
    </div>
  )
}

const SendAmsg = (props) => {

  props.
    return(
      <div className='w-[31vw] h-[24vh]'>
        <div className='h-[2vh] bg-cForeground text-right text-xs font-IBM font-regular italic pr-2'>
          as ASCII, 32 size char array
        </div>
        {/* put onChange here */}
        <textarea className='bg-white w-[100%] h-[22vh] overflow-y-auto overflow-x-hidden resize-none' />
      </div>
    );
}