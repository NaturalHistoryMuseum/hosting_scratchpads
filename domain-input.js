window.addEventListener('load', () => {
	for(const domainInput of document.querySelectorAll('.hosting_scratchpads_domain > input')) {
		domainInput.addEventListener('keyup', ({ target }) => {
			target.nextElementSitarget.style.display = target.value.indexOf('.') > -1 ? 'none' : 'inline';
		});
	}
});