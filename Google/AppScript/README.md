# 📊 Google/AppScript

>*Blah blah blah*

This script enables for GET and POST functionality for the Spreadsheet through deploying the script as a Web App in Google AppScripts. 

## 🛠 How to setup & run

1) After making a new Google Spreadsheet, **navigate to *Extensions* ⇒ *App Scripts***
2) **Name the application and copy and paste the code in** `code.js` **into the codespace.**
3) I'd suggest testing the code before deploying, **which can be done by selecting the function to run** (*in top bar*, ***should be `testPost` by default***) and **clicking the run button.** _The result should be seen in your spreadsheet._
4) Assuring that the test functions work, click **Deploy** ⇒ 'New Deployments' 
    - ***Select Type:*** Click the cog button and select **Web App**
    - ***Configuration*** 'Execute as Me', with 'Who has Acesss' to '*Anyone*'. 
5) Copy the generated Web App URL (this is connected to the deployment version and will change with different versions) 
    - The format should be somewhat similar to this:

```
Replace the brackets as needed!

https://script.google.com/macros/s/<$Your Deployment ID>/exec?beginDate=<$Some query begin date>&endDate=<$Some query end date>&limit=<$Some query limit (number)>
```

>I'd suggest using an application such as **Postman** to test the GET and POST functionalities of this script.