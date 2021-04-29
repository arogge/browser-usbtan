'using strict';

const inputFieldIds = [ 'txtTan', 'tanInputSelector' ]

var tanInputField

function findFormElements() {
  for (id of inputFieldIds) {
    let elem = document.getElementById(id)
    if (elem) {
      tanInputField = elem
      break
    }
  }
}

function handleResponse(tan) {
  console.debug(`TAN from backend: ${tan}`)

  if(tanInputField) {
    console.debug(tanInputField)
    console.debug(tanInputField.form)
    tanInputField.value = tan
    // tanInputField.form.submit()
  } else {
    console.warning("No input-field for TAN found, cannot set value.")
  }
}

function handleError(error) {
  console.log(`Error from background script: ${error}`);
}

if(document.readyState === 'loading') {
  document.addEventListener("DOMContentLoaded", findFormElements)
} else {
  findFormElements()
}

// register our custom events
document.addEventListener("flickercode_scraped", function(e) {
  console.debug(`Requesting TAN for flickercode: ${e.detail}`)
  browser.runtime.sendMessage({"flickercode": e.detail}).then(handleResponse, handleError);
})

// inject the scraper
var s = document.createElement('script');
s.src = chrome.runtime.getURL('flickercode_scraper.js');

(document.head || document.documentElement).appendChild(s);
