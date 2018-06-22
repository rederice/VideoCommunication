##Motion Interpolation

**All files should be in the same level of directory.**

*Method:*

```
Three Step Search

Linear motion prediction
```


Split video to sample part and comparable part:

```
	python3 halfvideo.py (original.YUV) (CIF type)
```

	-> Get "half_XXX.YUV" and "comp_XXX.YUV" files.

Interpolate image:

```
	python3 interpolate.py (half.YUV) (CIF type)
```

	-> Get a "result_XXX.YUV" video file.

draw psnr:

```
	python3 draw.py (original.YUV) (result.YUV) (CIF type)
```

	-> Get "XXX_psnr.png" image.