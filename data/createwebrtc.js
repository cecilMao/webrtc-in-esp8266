var Socket = new WebSocket('ws://' + window.location.hostname + ':81');

var conf = {iceServers: [{urls: []}]};
var pc = new RTCPeerConnection(conf);
var localStream, _fileChannel, chatEnabled,context,source,
	_chatChannel,sendFileDom = {}, 
	recFileDom={},receiveBuffer=[],
	receivedSize=0,
	file,
	bytesPrev=0; 
var JSONTEMP;

Socket.onmessage = function (evt) {
	console.log('remote onaddstream',evt.data);
	var obj=JSON.parse(evt.data);
	if (obj.type=="answer")

	{
		// remoteOffer.value=evt.data;
			var _remoteOffer = new RTCSessionDescription(obj);
			console.log('remoteOffer \n',_remoteOffer);
			pc.setRemoteDescription(_remoteOffer).then(function() {
					console.log('setRemoteDescription ok');
					if(_remoteOffer.type == "offer"){
				        pc.createAnswer().then(function(description){
				        	console.log('createAnswer 200 ok \n',description);
						    pc.setLocalDescription(description).then(function() {
						    }).catch(errHandler);	            	
				        }).catch(errHandler);				
					}
			}).catch(errHandler);}	
			else if (obj.type=="led")
			{
				if (obj.no=="1"){
				var e = document.getElementById("ledstatus1");
				if (obj.status =="ON") {
					e.checked = "checked";
				}
				else if (obj.status =="OFF") {
					e.checked = "";
				}}
				if (obj.no=="2"){
				var e = document.getElementById("ledstatus2");
				if (obj.status =="ON") {
					e.checked = "checked";
				}
				else if (obj.status =="OFF") {
					e.checked = "";
				}}
			}
		}
function errHandler(err){
	console.log(err);
}


navigator.mediaDevices.getUserMedia({audio:true,video:
	 { frameRate: { ideal: 10, max: 15 },
	width: {exact: 640}, 
	height: {exact: 480}
	//facingMode: { exact: "environment" }
	 }
}).then(stream=>{
	localStream = stream;
	// micused.innerHTML = localStream.getAudioTracks()[0].label;
	pc.addStream(stream);
	local.src = URL.createObjectURL(stream);
	local.muted=true;
}).catch(errHandler);



pc.onicecandidate = function(e){
	var cand = e.candidate;
	if(!cand){
		console.log('iceGatheringState complete',pc.localDescription.sdp);
		// localOffer.value = JSON.stringify(pc.localDescription);
		Socket.send(JSON.stringify(pc.localDescription));
	}else{
		console.log(cand.candidate);
	}
}
pc.oniceconnectionstatechange = function(){
	console.log('iceconnectionstatechange: ',pc.iceConnectionState);
}
pc.onaddstream = function(e){
  console.log('remote onaddstream',e.stream);
  remote.src = URL.createObjectURL(e.stream);
}
pc.onconnection = function(e){
	console.log('onconnection ',e);
}


localOfferSet.onclick = function(){
	
	pc.createOffer().then(des=>{
		console.log('createOffer ok ');
		pc.setLocalDescription(des).then( ()=>{
			setTimeout(function(){
				if(pc.iceGatheringState == "complete"){
					return;
				}else{
					console.log('after GetherTimeout');
					// localOffer.value = JSON.stringify(pc.localDescription);
					
				}
			},2000);
			console.log('setLocalDescription ok');
		}).catch(errHandler);
		// For chat
	}).catch(errHandler);
}

function LEDswitch1(){
var LEDswitchCheck1 = document.getElementById("ledstatus1").checked;
if(LEDswitchCheck1){
Socket.send(JSON.stringify({"type":"led","no":"1","status":"ON"})); // monitor serial lee ledon
}
else if(!LEDswitchCheck1) {
Socket.send(JSON.stringify({"type":"led","no":"1","status":"OFF"})); // monitor serial lee ledoff
}}

function LEDswitch2(){
var LEDswitchCheck2 = document.getElementById("ledstatus2").checked;
if(LEDswitchCheck2){
Socket.send(JSON.stringify({"type":"led","no":"2","status":"ON"})); // monitor serial lee ledon
}
else if(!LEDswitchCheck2) {
Socket.send(JSON.stringify({"type":"led","no":"2","status":"OFF"})); // monitor serial lee ledoff
}}