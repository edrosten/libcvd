namespace CVD{
	namespace Internal{
		namespace DoNotUse{
			extern "C" {
				void dgesvd_();
			}
			
			void dummy_lapack_function()
			{
				dgesvd_();
			}
		}
	}
}
