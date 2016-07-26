//
//  GGColumnVCtl.m
//  ThePeople
//
//  Created by aoyolo on 16/4/12.
//  Copyright © 2016年 Ms.Gao. All rights reserved.
//

#import "GGColumnVCtl.h"
#import "GGColumnCell.h"
#import "GGColumnModel.h"
#import "MJRefresh.h"

@interface GGColumnVCtl ()<UICollectionViewDelegate,UICollectionViewDataSource,UICollectionViewDelegateFlowLayout>

@property (nonatomic , strong)UICollectionView *collectionView;
@property (nonatomic , strong)NSMutableArray *dataArr;
@property (nonatomic , strong)UICollectionViewFlowLayout *flowLayout;

@end

@implementation GGColumnVCtl

#pragma mark 懒加载存放数据的数组
-(NSMutableArray *)dataArr{
    if (_dataArr == nil) {
        _dataArr = [[NSMutableArray alloc]init];
    }
    return _dataArr;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    [self reloadCollectionView];
    [self registerCell];
    [self httpRequest];
//    添加刷新控件
    [self.collectionView addHeaderWithTarget:self action:@selector(httpRequest)];
//    添加自动下拉控件
    [self.collectionView headerBeginRefreshing];
    
}

#pragma mark 添加UIControllerView
-(void)reloadCollectionView{
    UICollectionViewFlowLayout *flowLayout = [[UICollectionViewFlowLayout alloc] init];
    UICollectionView *collectionView = [[UICollectionView alloc] initWithFrame:self.view.bounds collectionViewLayout:flowLayout];
    self.flowLayout = flowLayout;
    self.collectionView = collectionView;
    self.collectionView.delegate = self;
    self.collectionView.dataSource = self;
    [self.view addSubview:self.collectionView];
    self.collectionView.backgroundColor = [UIColor colorWithRed:240/255.0 green:240/255.0 blue:240/255.0 alpha:1];
    self.collectionView.showsVerticalScrollIndicator= NO;
}

#pragma make -注册cell
- (void)registerCell{
    [self.collectionView registerNib:[UINib nibWithNibName:@"GGColumnCell" bundle:nil] forCellWithReuseIdentifier:@"GGColumnCell"];
}

#pragma mark -解析网络数据
- (void)httpRequest{
    AFHTTPRequestOperationManager *manager = [AFHTTPRequestOperationManager manager];
    [manager GET:ColumnUrl parameters:nil success:^(AFHTTPRequestOperation * _Nonnull operation, id  _Nonnull responseObject) {
        if (self.dataArr.count) {
            [self.dataArr removeAllObjects];
        }
        self.dataArr = [NSMutableArray array];
        self.dataArr = [NSMutableArray array];
        for (NSDictionary *dic in responseObject) {
            GGColumnModel *model = [[GGColumnModel alloc]init];
            model.image = dic[@"image"];
            model.name = dic[@"name"];
            [self.dataArr addObject:model];
        }
        [self.collectionView reloadData];
        [self.collectionView headerEndRefreshing];
    } failure:^(AFHTTPRequestOperation * _Nonnull operation, NSError * _Nonnull error) {
        NSLog(@"%@",error);
    }];
}

#pragma mark <UICollectionViewDataSource>
- (NSInteger)numberOfSectionsInCollectionView:(UICollectionView *)collectionView {
    return 1;
}


- (NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section {
    return self.dataArr.count;
}

-(CGSize)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout sizeForItemAtIndexPath:(NSIndexPath *)indexPath{
    return CGSizeMake((ScreenWidth-20)/3, (ScreenWidth-20)/9*4+30);
}

- (UICollectionViewCell *)collectionView:(UICollectionView *)collectionView cellForItemAtIndexPath:(NSIndexPath *)indexPath {
    static NSString *cellID = @"GGColumnCell";
    GGColumnCell *cell = [collectionView dequeueReusableCellWithReuseIdentifier:cellID forIndexPath:indexPath];
    cell.columnModel = self.dataArr[indexPath.row];
    return cell;
}

- (UIEdgeInsets)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout insetForSectionAtIndex:(NSInteger)section{
    return UIEdgeInsetsMake(5,5,5,5);
}
-(CGFloat)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout minimumInteritemSpacingForSectionAtIndex:(NSInteger)section{
    return 5;
}
-(CGFloat)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout minimumLineSpacingForSectionAtIndex:(NSInteger)section{
    return 5;
}
#pragma mark <UICollectionViewDelegate>


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
}

@end
