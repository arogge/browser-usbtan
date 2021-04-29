'use strict';

function notify(message) {
  browser.notifications.create("my-notification", {
    "type": "basic",
    "iconUrl": browser.extension.getURL("link.png"),
    "title": "You requested a TAN!",
    "message": message.detail
  });
}

function handleError(error) {
  console.log("Error: " + error)
}

function handleMessage(request, sender, sendResponse) {
  var sending = browser.runtime.sendNativeMessage(
    "usbtan",
    {
      flickercode: request.flickercode
    }
  )
  return sending.then(function(response) {
    console.log(response)
    return response.tan 
  }, handleError)
}

browser.runtime.onMessage.addListener(handleMessage);
