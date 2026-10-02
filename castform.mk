CASTFORMGFXDIR := graphics/pokemon/castform

$(CASTFORMGFXDIR)/front.4bpp: $(CASTFORMGFXDIR)/normal/front.4bpp \
                              $(CASTFORMGFXDIR)/sunny/front.4bpp \
                              $(CASTFORMGFXDIR)/rainy/front.4bpp \
                              $(CASTFORMGFXDIR)/snowy/front.4bpp
	@cat $^ >$@

$(CASTFORMGFXDIR)/back.4bpp: $(CASTFORMGFXDIR)/normal/back.4bpp \
                             $(CASTFORMGFXDIR)/sunny/back.4bpp \
                             $(CASTFORMGFXDIR)/rainy/back.4bpp \
                             $(CASTFORMGFXDIR)/snowy/back.4bpp
	@cat $^ >$@

$(CASTFORMGFXDIR)/normal.gbapal: $(CASTFORMGFXDIR)/normal/normal.gbapal \
                                 $(CASTFORMGFXDIR)/sunny/normal.gbapal \
                                 $(CASTFORMGFXDIR)/rainy/normal.gbapal \
                                 $(CASTFORMGFXDIR)/snowy/normal.gbapal
	@cat $^ >$@

$(CASTFORMGFXDIR)/shiny.gbapal: $(CASTFORMGFXDIR)/normal/shiny.gbapal \
                                $(CASTFORMGFXDIR)/sunny/shiny.gbapal \
                                $(CASTFORMGFXDIR)/rainy/shiny.gbapal \
                                $(CASTFORMGFXDIR)/snowy/shiny.gbapal
	@cat $^ >$@
