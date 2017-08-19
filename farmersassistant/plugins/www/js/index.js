document.addEventListener('deviceready', onDeviceReady, false);

var data1="0";
var data2="0";
var data3="0";
var data4="0";
var data5="0";
var data6="NO";
var data7="NO";

var test="fail";

function onDeviceReady() {
      console.log("Device Ready");

      $(document).ready(function() {

        ajaxCall();

        setInterval(function() {
            ajaxCall();
            console.log("Refreshed Automatically");
        }, 10000);


          $("#refresh").click(function(){
              ajaxCall();
          });

          $("#switchon").click(function(){
              ajaxOnCall();
          });

          $("#switchoff").click(function(){
              ajaxOffCall();
          });
      });

  }



  function ajaxCall()
  {
    $.ajax({
       url:"http://54.174.95.66/fetch.php",
       dataType: 'jsonp', // Notice! JSONP <-- P (lowercase)
       success:function(json){
         // do stuff with json (in this case an array)
          test=json[0];
          if(test=="pass")
          {
          data1=json[1];
          data2=json[2];
          data3=json[3];
          data4=json[4];
          data5=json[5];
          data6=json[6];
          data7=json[7];

          saveData();
        }

       },
       error:function(){
            console.log("error");
            test="fail";

            getData();

       }
    });

    showData();
  }

  function ajaxOnCall()
  {
    $.ajax({
       url:"http://54.174.95.66/irrigateon.php",
       dataType: 'jsonp', // Notice! JSONP <-- P (lowercase)
       success:function(json){
         // do stuff with json (in this case an array)
          test=json[0];
          if(test=="pass")
          {
              localStorage.setItem('data7', "YES");
              data7 = localStorage.getItem("data7");
              document.getElementById("data7").innerHTML = data7;
          }
       },
       error:function(){
            console.log("error");
            test="fail";
       }
    });

  }

  function ajaxOffCall()
  {
    $.ajax({
       url:"http://54.174.95.66/irrigateoff.php",
       dataType: 'jsonp', // Notice! JSONP <-- P (lowercase)
       success:function(json){
         // do stuff with json (in this case an array)
          test=json[0];
          if(test=="pass")
          {
              localStorage.setItem('data7', "NO");
              data7 = localStorage.getItem("data7");
              document.getElementById("data7").innerHTML = data7;
          }
       },
       error:function(){
            console.log("error");
            test="fail";
       }
    });

  }

  function saveData()
  {
    //alert("save data");
      localStorage.setItem('data1', data1);
      localStorage.setItem('data2', data2);
      localStorage.setItem('data3', data3);
      localStorage.setItem('data4', data4);
      localStorage.setItem('data5', data5);
      localStorage.setItem('data6', data6);
      localStorage.setItem('data7', data7);

  }


  function getData()
  {
    data1 = localStorage.getItem("data1");
    data2 = localStorage.getItem("data2");
    data3 = localStorage.getItem("data3");
    data4 = localStorage.getItem("data4");
    data5 = localStorage.getItem("data5");
    data6 = localStorage.getItem("data6");
    data7 = localStorage.getItem("data7");
  }


  function showData()
  {
    //alert("show data");
    if(test=="fail")
    {
      console.log("test failed");
      getData();
    }
    else
    {
      test="fail";
    }

    document.getElementById("data1").innerHTML = data1;
    document.getElementById("data2").innerHTML = data2;
    document.getElementById("data3").innerHTML = data3;
    document.getElementById("data4").innerHTML = data4;
    document.getElementById("data5").innerHTML = data5;
    document.getElementById("data6").innerHTML = data6;
    document.getElementById("data7").innerHTML = data7;
  }
