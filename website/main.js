var currentNavbarEl = "";
var defaultDelay = 750, defaultVerticalOffset = 40;
var startedByFunc = false;

function scrollToElement(obj, yOffset, delay)
{
	var offset = obj.offset();

	startedByFunc = true;
	$('html, body').animate({
	    scrollTop: offset.top - yOffset + "px"
	}, delay);
}

function NavigationElement(name, sectionID)
{
	this.name = name;
	this.sectionID = sectionID;
	this.delay = defaultDelay;
	this.offset = defaultVerticalOffset;

	this.onClick = function()
	{
		// Scroll to the anchor
		scrollToElement($("#" + this.sectionID + " .section_goto"), this.offset, this.delay);
		currentNavbarEl = "navel_" + this.sectionID;
	}

	// Add this element to the navigation bar
	var navbar = document.getElementById("navbar");

	if(navbar == null)
	{
		console.log("Unable to add element to navigation bar.");
	}
	else
	{
		var data = "<div class=\"navbar_element\" id=\"navel_" + this.sectionID + "\" onclick=\"" + this.sectionID + ".onClick();\"><label style=\"z-index: 3;\" class=\"navbar_el_name\">" + this.name + "</label></div>";
		navbar.innerHTML = navbar.innerHTML + "\n" + data;
	}
}

function onWindowScroll()
{
	$(".navbar_element").each(function()
	{
		var os = document.body.scrollTop;
		console.log(os);

		$(this).removeClass("navbar_element_active", 250);

		var secs = $(".main_section");

		for(var i = 0; i < secs.length; i++)
		{
			if(this.id == "navel_" + secs[i].id)
			{
				if((os >= $(secs[i]).offset().top - defaultVerticalOffset - 150 && os < $(secs[i]).offset().top + $(secs[i]).height() - defaultVerticalOffset - 150) 
					|| (i == 0 && os < $(secs[i]).offset().top - defaultVerticalOffset - 150)
					|| (i == secs.length - 1 && os > $(secs[i]).offset().top + $(secs[i]).height() - defaultVerticalOffset - 150))
					$(this).addClass("navbar_element_active", 250);
			}
		}
	});

	$.doTimeout("scroll", 100, function()
	{
		if(startedByFunc)
		{
			console.log("yesh");

			$(".navbar_element").each(function() 
			{
				$(this).removeClass("navbar_element_active", 250);
			});

			$("#" + currentNavbarEl).addClass("navbar_element_active", 250);
		}

		startedByFunc = false;
	});
}

function onWindowLoad()
{
	var elems = $(".section_title");

	for(var i = 0; i < elems.length; i++)
	{
		elems[i].onclick = function()
		{
			scrollToElement($("#" + this.parentNode.getAttribute("name") + "_goto"), defaultVerticalOffset, defaultDelay);
			currentNavbarEl = "navel_" + this.parentNode.id;
		}
	}

	// Create all of the navigation elements
	elems = $(".main_section");

	$(window).scroll(onWindowScroll);

	elems.each(function()
	{
		window[this.id] = new NavigationElement(this.getAttribute("name"), this.id);
	});

	$(elems[0]).css("margin-top", "0px");

	elems = $(".navbar_element");
	$(elems[0]).addClass("navbar_element_active", 0);
	currentNavbarEl = elems[0].id;

	window.onresize = function()
	{	
		// Adjust navigation bar size
		var left = ($("#body").outerWidth() - $("#main").outerWidth()) / 2  - $("#navbar").outerWidth();
		$("#navbar").css("left", left + "px");
	};
	window.onresize();
}